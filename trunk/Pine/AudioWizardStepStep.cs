using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pine
{
    public class AudioWizardStepStep
    {
        private readonly IAudioConfiguration _audioConfiguration;
        private readonly IRadioFactory _radioFactory;
        private IRadio _radio;

        public AudioWizardStepStep(IAudioConfiguration audioConfiguration, IRadioFactory radioFactory)
        {
            _audioConfiguration = audioConfiguration;
            _radioFactory = radioFactory;
        }

        public string InputId
        {
            get { throw new NotImplementedException(); }
            set { _audioConfiguration.InputId = value; }
        }

        public string OutputId
        {
            get { throw new NotImplementedException(); }
            set { _audioConfiguration.OutputId = value; }
        }

        public bool CheckParameters
        {
            get { throw new NotImplementedException(); }
            set
            {
                if (value)
                {
                    _radio = _radioFactory.CreateRadio(_audioConfiguration);
                }
                else
                {
                    _radio.Dispose();
                }

            }
        }

        public double Frequency
        {
            get { throw new NotImplementedException(); }
            set { _radio.Frequency = value; }
        }
    }
}
