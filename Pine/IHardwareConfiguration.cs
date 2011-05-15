using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pine
{
    public interface IAudioConfiguration
    {
        string InputId { get; set; }
        string OutputId { get; set; }
    }
}
