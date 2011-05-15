using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Moq;
using NUnit.Framework;

namespace Pine
{
    class RadioWizardStepTest
    {
        [Test]
        public void RadioStationsSetToRadioConfiguration()
        {
            var radio = new Mock<IRadio>();
            var radioConfiguration = new Mock<IRadioConfiguration>();
            radioConfiguration.SetupProperty(p => p.RadioStations);
            RadioWizardStep radioWizardStep = new RadioWizardStep(radio.Object, radioConfiguration.Object);
            RadioStation[] radioStations = new RadioStation[0];

            radioWizardStep.RadioStations = radioStations;

            Assert.AreEqual(radioStations, radioConfiguration.Object.RadioStations);
        }
    }
}
