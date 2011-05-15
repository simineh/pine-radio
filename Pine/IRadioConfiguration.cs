using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pine
{
    public interface IRadioConfiguration
    {
        RadioStation[] RadioStations { get; set; }
    }
}
