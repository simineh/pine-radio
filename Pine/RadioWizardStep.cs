using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pine
{
    public class RadioWizardStep
    {
        private readonly IRadio _radio;
        private readonly IRadioConfiguration _radioConfiguration;

        public RadioWizardStep(IRadio radio, IRadioConfiguration radioConfiguration)
        {
            _radio = radio;
            _radioConfiguration = radioConfiguration;
        }

        public RadioStation[] RadioStations
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }
    }
}
