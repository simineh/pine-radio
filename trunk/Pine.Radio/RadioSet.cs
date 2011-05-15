using System;
using Pine.Audio;

namespace Pine.Radio
{
    public class RadioSet: IDisposable
    {
        private readonly RadioDevice _radioDevice;
        private readonly SoundMuter _soundMuter;
        private double _frequency;

        public RadioSet(RadioDevice radioDevice, SoundMuter soundMuter)
        {
            _radioDevice = radioDevice;
            _soundMuter = soundMuter;

            _radioDevice.TurnOn();
        }

        public double Frequency
        {
            get { return _frequency; }
            set {
                _frequency = value;
                _radioDevice.SetFrequency(_frequency);
            }
        }

        public bool Mute
        {
            get { return _soundMuter.Mute; }
            set { _soundMuter.Mute = value; }
        }

        public void Dispose()
        {
            _radioDevice.Dispose();
            _soundMuter.Dispose();
        }
    }
}
