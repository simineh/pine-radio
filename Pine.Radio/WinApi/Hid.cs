using System;
using System.Runtime.InteropServices;

namespace Pine.Radio.WinApi
{
    public class Hid
    {
        [DllImport("hid.dll", SetLastError = true)]
        public static extern void HidD_GetHidGuid(ref Guid hidGuid);
    }
}