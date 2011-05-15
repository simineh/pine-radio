using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Pine.Audio;

namespace Pine
{
    /// <summary>
    /// Interaction logic for Settings.xaml
    /// </summary>
    public partial class Settings : Window
    {
        public Settings()
        {
            InitializeComponent();
            AudioDevices audioDevices =
                new AudioDevices()
                {
                    InputAudioDevices = SoundRoute.GetAudioDevices(AudioDeviceType.Input),
                    OutputAudioDevices = SoundRoute.GetAudioDevices(AudioDeviceType.Output)
                };
            DataContext = audioDevices;
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
        }

        public AudioDevice InputAudioDevice
        {
            get { return (AudioDevice) listBox1.SelectedItem; }
        }

        public AudioDevice OutputAudioDevice
        {
            get { return (AudioDevice) listBox2.SelectedItem; }
        }
    }

    struct AudioDevices
    {
        public AudioDevice[] InputAudioDevices { get; set; }
        public AudioDevice[] OutputAudioDevices { get; set; }
    }
}
