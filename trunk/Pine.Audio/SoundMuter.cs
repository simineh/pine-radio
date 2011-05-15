using System;

namespace Pine.Audio
{
    public class SoundMuter: IDisposable
    {
        private readonly string _inputAudioDeviceId;
        private readonly string _outputAudioDeviceId;
        private IntPtr _transferId = IntPtr.Zero;

        public SoundMuter(string inputAudioDeviceId, string outputAudioDeviceId)
        {
            _inputAudioDeviceId = inputAudioDeviceId;
            _outputAudioDeviceId = outputAudioDeviceId;
        }

        public bool Mute
        {
            get { return _transferId == IntPtr.Zero; }
            set
            {
                if (value && _transferId == IntPtr.Zero ||
                    !value && _transferId != IntPtr.Zero)
                {
                    return;
                }

                if (value)
                {
                    SoundRoute.StopSound(_transferId);
                    _transferId = IntPtr.Zero;
                }
                else
                {
                    _transferId = SoundRoute.StartSound(_inputAudioDeviceId, _outputAudioDeviceId);
                }
            }
        }

        public void Dispose()
        {
            if (_transferId != IntPtr.Zero)
            {
                SoundRoute.StopSound(_transferId);
            }
        }


    }
}
