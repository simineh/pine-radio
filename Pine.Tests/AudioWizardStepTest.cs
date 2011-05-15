using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Moq;
using NUnit.Framework;

namespace Pine
{
    public class AudioWizardStepTest
    {
        [Test]
        public void InputAndOutputWritesToConfiguration()
        {
            var configurationMock = new Mock<IAudioConfiguration>();
            var radioFactoryMock = new Mock<IRadioFactory>();
            configurationMock.SetupProperty(p => p.InputId);
            configurationMock.SetupProperty(p => p.OutputId);
            IAudioConfiguration audioConfiguration = configurationMock.Object;
            var setupWizard = new AudioWizardStepStep(audioConfiguration, radioFactoryMock.Object);

            setupWizard.InputId = "123456";
            setupWizard.OutputId = "678910";

            setupWizard.CheckParameters = true;

            Assert.AreEqual("123456", audioConfiguration.InputId);
            Assert.AreEqual("678910", audioConfiguration.OutputId);
        }

        [Test]
        public void EnabelingCheckParametersMode_CreateRadioCalled()
        {
            var configurationMock = new Mock<IAudioConfiguration>();
            IAudioConfiguration audioConfiguration = configurationMock.Object;
            var radioFactoryMock = new Mock<IRadioFactory>();
            var setupWizard = new AudioWizardStepStep(audioConfiguration, radioFactoryMock.Object);

            setupWizard.CheckParameters = true;

            radioFactoryMock.Verify(f => f.CreateRadio(audioConfiguration));
        }

        [Test]
        public void DisableingCheckParametersMode_DisposesRadio()
        {
            var radioMock = new Mock<IRadio>();

            var configurationMock = new Mock<IAudioConfiguration>();
            IAudioConfiguration audioConfiguration = configurationMock.Object;
            var radioFactoryMock = new Mock<IRadioFactory>();
            radioFactoryMock.Setup(f => f.CreateRadio(It.IsAny<IAudioConfiguration>())).Returns(radioMock.Object);
            var setupWizard = new AudioWizardStepStep(audioConfiguration, radioFactoryMock.Object);

            setupWizard.CheckParameters = true;
            setupWizard.CheckParameters = false;

            radioMock.Verify(f => f.Dispose());
        }

        //TODO: добавить тесты на ошибочный повторый вызов CheckParameters

        [Test]
        public void SetFrequency_FrequencySetOnCurrentRadio()
        {
            var radioMock = new Mock<IRadio>();
            radioMock.SetupProperty(p => p.Frequency);
            IRadio radio = radioMock.Object;

            var configurationMock = new Mock<IAudioConfiguration>();
            IAudioConfiguration audioConfiguration = configurationMock.Object;
            var radioFactoryMock = new Mock<IRadioFactory>();
            radioFactoryMock.Setup(f => f.CreateRadio(It.IsAny<IAudioConfiguration>())).Returns(radioMock.Object);
            var setupWizard = new AudioWizardStepStep(audioConfiguration, radioFactoryMock.Object);

            setupWizard.CheckParameters = true;
            setupWizard.Frequency = 101.7;

            Assert.AreEqual(101.7, radio.Frequency);
        }

        //TODO: добавить тесты на случай, когда частота устанавливается не в редиме проверки
    }
}
