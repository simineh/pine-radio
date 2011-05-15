using NUnit.Framework;

namespace Pine.Radio
{
    public class FrequencyConverterTest
    {
        //[TestCase(108.05, Result = 0x21D2)]
        //[TestCase(108.00, Result = 0x21CE)]
        //[TestCase(107.95, Result = 0x21CA)]
        //[TestCase(107.90, Result = 0x21C6)]
        //[TestCase(107.85, Result = 0x21C2)]
        //[TestCase(076.05, Result = 0x17D2)]
        //[TestCase(076.10, Result = 0x17D6)]
        //public ushort Convert_NormalFrequences(double frequency)
        //{
        //    FrequencyConverter converter = new FrequencyConverter();
        //    return converter.Convert(frequency);
        //}

        [Test]
        public void convert_test1()
        {
            assertConvert(108.05, 0x21D2);
        }

        [Test]
        public void convert_test2()
        {
            assertConvert(76.05, 0x17D2);
        }

        [Test]
        public void convert_test3()
        {
            assertConvert(76.10, 0x17D6);
        }

        [Test]
        public void convert_test4()
        {
            assertConvert(101.75, 0x1FD9);
        }

        [Test]
        public void convert_test5()
        {
            assertConvert(103.75, 0x207A);
        }

        private void assertConvert(double frequency, ushort expected)
        {
            FrequencyConverter converter = new FrequencyConverter();
            ushort actual = converter.Convert(frequency);
            Assert.AreEqual(expected, actual);
        }

        [Test]
        public void ConvertUshortToBytes_FFAA_ArrayOfFFAndAA()
        {
            ushort number = 0xFFAA;
            byte[] expected = {0xFF, 0xAA};
            FrequencyConverter converter = new FrequencyConverter();

            byte[] actual = converter.ConvertUshortToBytes(number);

            Assert.AreEqual(expected, actual);
        }

        [Test]
        public void ConvertUshortToBytes_AB34_ArrayOfABAnd34()
        {
            ushort number = 0xAB34;
            byte[] expected = { 0xAB, 0x34 };
            FrequencyConverter converter = new FrequencyConverter();

            byte[] actual = converter.ConvertUshortToBytes(number);

            Assert.AreEqual(expected, actual);
        }
    }
}
