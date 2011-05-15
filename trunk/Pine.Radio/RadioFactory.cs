using System;
using System.Collections.Generic;

namespace Pine.Radio
{
    public class RadioFactory
    {
        public RadioDevice Create()
        {
            DeviceFinder deviceFinder = new DeviceFinder();
            IList<string> devicePaths = deviceFinder.FindDevices();
            IntPtr deviceHandle = deviceFinder.OpenDevice(devicePaths[0]);
            RadioDevice radioDevice = new RadioDevice(deviceHandle);
            return radioDevice;
        }
    }
}
