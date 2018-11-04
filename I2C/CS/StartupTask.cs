using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Devices.I2c;
using Windows.System.Threading;

using static System.Diagnostics.Debug;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace CS
{
    public sealed class StartupTask : IBackgroundTask
    {
		BackgroundTaskDeferral deferral;
		private ThreadPoolTimer timer;
		I2cDevice sensor;

		const int ADDR = 8;

		public async void Run(IBackgroundTaskInstance taskInstance)
        {
			deferral = taskInstance.GetDeferral();

			//Ox40 was determined by looking at the datasheet for the device
			var controller = await I2cController.GetDefaultAsync();
			sensor = controller.GetDevice(new I2cConnectionSettings(8));
			timer = ThreadPoolTimer.CreatePeriodicTimer(Timer_Tick, TimeSpan.FromSeconds(10));

			// can't do this, ends the app deferral.Complete();
		}

		private void Timer_Tick(ThreadPoolTimer timer)
		{
			byte [] emptyBuffer = new byte[0];
			byte[] byteBuffer = new byte[1];
			byte[] wordBuffer = new byte[2];

			sensor.Write(byteBuffer);
			sensor.Read(byteBuffer);
			WriteLine($"Got first byte of {byteBuffer[0]}");

			byteBuffer[0] = 0x81;
			I2cTransferResult result = sensor.WriteReadPartial(byteBuffer, wordBuffer);
			WriteLine($"Result is {result.Status} Bytes transferred is {result.BytesTransferred} temp is 0x{wordBuffer[1]:X2}{wordBuffer[0]:X2}");


			byteBuffer[0] = 0x82;
			result = sensor.WriteReadPartial(byteBuffer, wordBuffer);
			WriteLine($"Result is {result.Status} Bytes transferred is {result.BytesTransferred} light is 0x{wordBuffer[1]:X2}{wordBuffer[0]:X2}");

		}
	}
}
