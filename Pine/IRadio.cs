using System;

namespace Pine
{
    public interface IRadio : IDisposable
    {
        double Frequency { get; set; }
    }
}