// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Devices.Gpio;
using Windows.System.Threading;



namespace RgbLed
{
    /// <summary>
    /// little sample that drives an RGB LED for 8 colors (including off)
    /// </summary>
    public sealed class StartupTask : IBackgroundTask
    {
        BackgroundTaskDeferral deferral;
        private const int LED_REDPIN = 4;  
        private const int LED_GREENPIN = 17; 
        private const int LED_BLUEPIN = 27;

        private GpioPin pinRed;
        private GpioPin pinGreen;
        private GpioPin pinBlue;
        private ThreadPoolTimer timer;

        private int i = 0;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            deferral = taskInstance.GetDeferral();
            InitGPIO();
            timer = ThreadPoolTimer.CreatePeriodicTimer(Timer_Tick, TimeSpan.FromMilliseconds(500));
            
        }
        private void InitGPIO()
        {
            pinRed = GpioController.GetDefault().OpenPin(LED_REDPIN);
            pinRed.Write(GpioPinValue.Low);
            pinRed.SetDriveMode(GpioPinDriveMode.Output);

            pinGreen = GpioController.GetDefault().OpenPin(LED_GREENPIN);
            pinGreen.Write(GpioPinValue.Low);
            pinGreen.SetDriveMode(GpioPinDriveMode.Output);

            pinBlue = GpioController.GetDefault().OpenPin(LED_BLUEPIN);
            pinBlue.Write(GpioPinValue.Low);
            pinBlue.SetDriveMode(GpioPinDriveMode.Output);
        }

        private void Timer_Tick(ThreadPoolTimer timer)
        {
            if (i > 7)
                i = 0;
            i++;

            pinRed.Write((i & 1) != 0 ? GpioPinValue.High : GpioPinValue.Low);
            pinGreen.Write((i & 2) != 0? GpioPinValue.High : GpioPinValue.Low);
            pinBlue.Write((i & 4) != 0 ? GpioPinValue.High : GpioPinValue.Low);
        }
    }
}
