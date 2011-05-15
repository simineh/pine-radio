using System;

namespace Pine.Radio
{
    public class FrequencyConverter
    {
        public ushort Convert(double frequency)
        {
            int start = 0x17D2;
            double startFreq = 76.05;
            int count = (int) Math.Round((frequency - startFreq)/0.05);
            int result = start + count*4;
            return (ushort) result;
        }

        public byte[] ConvertUshortToBytes(ushort frequencyData)
        {
            byte[] bytes = new byte[2];
            bytes[0] = (byte) ((frequencyData & 0xFF00) >> 8);
            bytes[1] = (byte) (frequencyData & 0x00FF);
            return bytes;
        }
    }
}
