using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using Pine.Audio;
using Pine.Radio;

namespace Pine
{
    /// <summary>
    /// Interaction logic for PineApplication.xaml
    /// </summary>
    public partial class PineApplication : Application
    {
        private RadioSet _radio;

        private void ApplicationStartup(object sender, StartupEventArgs e)
        {
            PineWindow pineWindow = new PineWindow();
            Settings settings = new Settings();

            settings.ShowDialog();

            if (settings.DialogResult == true)
            {
                _radio = CreateRadio(settings.InputAudioDevice.Id, settings.OutputAudioDevice.Id);
            }
            else
            {
                Shutdown();
            }

            pineWindow.DataContext = _radio;
            pineWindow.Show();
        }

        private RadioSet CreateRadio(string inputId, string outputId)
        {

            SoundMuter soundMuter = new SoundMuter(inputId, outputId);

            RadioFactory radioFactory = new RadioFactory();
            RadioDevice radioDevice = radioFactory.Create();

            return new RadioSet(radioDevice, soundMuter);
        }

        private void ApplicationExit(object sender, ExitEventArgs e)
        {
            if (_radio != null)
            {
                _radio.Dispose();
            }
        }
    }
}
