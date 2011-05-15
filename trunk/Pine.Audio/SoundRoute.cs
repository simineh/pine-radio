using System;
using System.Runtime.InteropServices;

namespace Pine.Audio
{
    public static class SoundRoute
    {
        [DllImport("audio.dll", CallingConvention = CallingConvention.Cdecl)]
        private unsafe static extern void GetAudioDevices(AudioDeviceType audioDeviceType, out TestStruct** structs, out int deviceType);

        [DllImport("audio.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern unsafe void DisposeAudioDevices(TestStruct** audioDevices, int audioDevicesCount);

        [DllImport("audio.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr StartSound([MarshalAs(UnmanagedType.LPWStr)]string inputId, [MarshalAs(UnmanagedType.LPWStr)]string outputId);

        [DllImport("audio.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void StopSound(IntPtr soundTransferId);

        public static AudioDevice[] GetAudioDevices(AudioDeviceType audioDeviceType)
        {
            AudioDevice[] audioDevices = null;
            unsafe
            {
                TestStruct** structs;
                int devicesCount;

                GetAudioDevices(audioDeviceType, out structs, out devicesCount);
                audioDevices = new AudioDevice[devicesCount];
                for (int i = 0; i < devicesCount; ++i)
                {
                    AudioDevice audioDevice = new AudioDevice
                                                  {
                                                      Id = Marshal.PtrToStringUni(structs[i]->Id),
                                                      Description = Marshal.PtrToStringUni(structs[i]->Description)
                                                  };
                    audioDevices[i] = audioDevice;
                }
                DisposeAudioDevices(structs, devicesCount);
            }
            return audioDevices;
        }

    }

    [StructLayout(LayoutKind.Sequential)]
    struct TestStruct
    {
        public IntPtr Id;
        public IntPtr Description;
    };

    public struct AudioDevice
    {
        public string Id { get; set; }
        public string Description { get; set; }
    }

    public enum AudioDeviceType : int
    {
        Output = 0,
        Input = 1
    }
}
