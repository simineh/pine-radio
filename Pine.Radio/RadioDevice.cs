using System;
using System.Threading;
using Pine.Radio.WinApi;

namespace Pine.Radio
{
    public class RadioDevice: IDisposable
    {
        private readonly IntPtr _deviceHandle;
        private readonly FrequencyConverter _frequencyConverter = new FrequencyConverter();

        public RadioDevice(IntPtr deviceHandle)
        {
            _deviceHandle = deviceHandle;
        }

        public void TurnOn()
        {
            byte[][] data = new[]
                                {
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x08, 0xB0, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0x3D, 0x32, 0x0F, 0x05, 0x3D, 0x32, 0x0F, 0x05},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xAE, 0x00, 0x00, 0x00},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xAB, 0x00, 0x00, 0x00}
                                };
            WriteReportsToDevice(data);
        }

        public void SetFrequency(double frequency)
        {
            ushort frequencyData = _frequencyConverter.Convert(frequency);
            byte[] frequencyArray = _frequencyConverter.ConvertUshortToBytes(frequencyData);

            byte[][] data = new[]
                                {
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x03, 0xA4, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0xFF, 0xFF, 0x01, 0x00, 0xAB, 0x00, 0x00, 0x00}
                                };

            data[1][1] = frequencyArray[0];
            data[1][2] = frequencyArray[1];

            WriteReportsToDevice(data);
        }

        public void TurnOff()
        {
            byte[][] data = new[]
                                {
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xA8, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xA8, 0x00, 0x00, 0x08},
                                    new byte[] {0x00, 0x00, 0x55, 0xAA, 0x00, 0xAB, 0x01, 0x00, 0x00}
                                };
            WriteReportsToDevice(data);
        }

        private void WriteReportsToDevice(byte[][] data)
        {
            for (int i = 0; i < data.Length; i++)
            {
                byte[] t = data[i];
                WriteToDevice(_deviceHandle, t);
                Thread.Sleep(250);
            }
        }

        private unsafe void WriteToDevice(IntPtr hidHandle, byte[] data)
        {
            uint bufferSize = 9;

            fixed (byte* buffer = data)
            {
                uint bytesWritten;
                bool success = Kernel32.WriteFile(hidHandle, buffer, bufferSize, out bytesWritten, IntPtr.Zero);
            }
        }

        public void Dispose()
        {
            TurnOff();
            Kernel32.CloseHandle(_deviceHandle);
        }
    }
}