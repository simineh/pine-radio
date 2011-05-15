using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using Pine.Radio.WinApi;

namespace Pine.Radio
{
    public class DeviceFinder
    {
        public IList<string> FindDevices()
        {
            IList<string> deviceNames = new List<string>();

            Guid hidGuid = new Guid();

            Hid.HidD_GetHidGuid(ref hidGuid);

            uint flags = (uint) (DiGetClassFlags.DIGCF_PRESENT | DiGetClassFlags.DIGCF_DEVICEINTERFACE);
            IntPtr pnpHandle = IntPtr.Zero;
            try
            {
                pnpHandle = SetupApi.SetupDiGetClassDevs(ref hidGuid, IntPtr.Zero, IntPtr.Zero, flags);

                SP_DEVICE_INTERFACE_DATA data = new SP_DEVICE_INTERFACE_DATA();
                data.cbSize = Marshal.SizeOf(data);
                int devIndex = 0;
                bool success = false;
                do
                {
                    success = SetupApi.SetupDiEnumDeviceInterfaces(pnpHandle, IntPtr.Zero, ref hidGuid, devIndex,
                                                                   ref data);
                    if (success)
                    {
                        SP_DEVINFO_DATA devData = new SP_DEVINFO_DATA();
                        devData.cbSize = Marshal.SizeOf(devData);

                        int bytesReturned;

                        SetupApi.SetupDiGetDeviceInterfaceDetail(pnpHandle, ref data, IntPtr.Zero, 0, out bytesReturned,
                                                                 ref devData);

                        if (bytesReturned != 0 && Marshal.GetLastWin32Error() == ErrorCodes.ERROR_INSUFFICIENT_BUFFER)
                        {
                            unsafe
                            {
                                IntPtr deviceInterfaceDetailData = Marshal.AllocHGlobal(bytesReturned);
                                SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceData =
                                    (SP_DEVICE_INTERFACE_DETAIL_DATA*) deviceInterfaceDetailData.ToPointer();
                                interfaceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

                                if (SetupApi.SetupDiGetDeviceInterfaceDetail(pnpHandle, ref data,
                                                                             deviceInterfaceDetailData,
                                                                             bytesReturned,
                                                                             out bytesReturned, ref devData))
                                {
                                    IntPtr devicePathTchar = IntPtr.Add(deviceInterfaceDetailData, sizeof (int));
                                    string devicePath = Marshal.PtrToStringAuto(devicePathTchar);
                                    deviceNames.Add(devicePath);
                                }

                                Marshal.FreeHGlobal(deviceInterfaceDetailData);
                            }
                        }
                    }
                    ++devIndex;
                } while (success);

                return deviceNames;
            }
            finally
            {
                if (pnpHandle != IntPtr.Zero)
                {
                    SetupApi.SetupDiDestroyDeviceInfoList(pnpHandle);
                }
            }
        }

        public IntPtr OpenDevice(string devicePath)
        {
            IntPtr hidHandle = Kernel32.CreateFile(devicePath, EFileAccess.GenericRead | EFileAccess.GenericWrite,
                                                   EFileShare.Read | EFileShare.Write, IntPtr.Zero,
                                                   ECreationDisposition.OpenExisting, EFileAttributes.None, IntPtr.Zero);
            if (hidHandle.Equals(SpecialHandles.INVALID_HANDLE_VALUE))
            {
                throw new Exception("Не удалось открыть устройство");
            }

            return hidHandle;
        }

        public void SendCommands(IntPtr hidHandle)
        {
            byte[][] data = new[]
                                {
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x08, 0xB0, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0x3D, 0x32, 0x0F, 0x05, 0x3D, 0x32, 0x0F, 0x05},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xAE, 0x00, 0x00, 0x00},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xAB, 0x00, 0x00, 0x00},

                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x03, 0xA4, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0x1D, 0x6E, 0x01, 0x00, 0xAB, 0x00, 0x00, 0x00}
                                };
            for (int i = 0; i < data.Length; i++)
            {
                byte[] t = data[i];
                writeToDevice(hidHandle, t);
                Console.WriteLine(i);
                Thread.Sleep(1000);
            }
        }

        private unsafe void writeToDevice(IntPtr hidHandle, byte[] data)
        {
            uint bufferSize = 9;

            fixed (byte* buffer = data)
            {
                uint bytesWritten;
                bool success = Kernel32.WriteFile(hidHandle, buffer, bufferSize, out bytesWritten, IntPtr.Zero);
            }
        }
    }
}
