// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Devices.Gpio;
using Windows.System.Threading;
using System.Diagnostics;

namespace DCMotor
{
    /// <summary>
    /// little sample that drives an RGB LED for 8 colors (including off)
    /// </summary>
    public sealed class StartupTask : IBackgroundTask
    {
        BackgroundTaskDeferral deferral;
        private const int ENABLE_PIN = 4;  
        private const int FORWARD_PIN = 17; 
        private const int BACKWARD_PIN = 27;

        private GpioPin pinEnable;
        private GpioPin pinForward;
        private GpioPin pinBackward;
        private ThreadPoolTimer timer;

        private int i = 0;

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            deferral = taskInstance.GetDeferral();
            InitGPIO();
            timer = ThreadPoolTimer.CreatePeriodicTimer(Timer_Tick, TimeSpan.FromMilliseconds(3000));
            
        }
        private void InitGPIO()
        {
            pinEnable = GpioController.GetDefault().OpenPin(ENABLE_PIN);
            pinEnable.Write(GpioPinValue.Low);
            pinEnable.SetDriveMode(GpioPinDriveMode.Output);

            pinForward = GpioController.GetDefault().OpenPin(FORWARD_PIN);
            pinForward.Write(GpioPinValue.Low);
            pinForward.SetDriveMode(GpioPinDriveMode.Output);

            pinBackward = GpioController.GetDefault().OpenPin(BACKWARD_PIN);
            pinBackward.Write(GpioPinValue.Low);
            pinBackward.SetDriveMode(GpioPinDriveMode.Output);
        }

        private void Timer_Tick(ThreadPoolTimer timer)
        {
            if (i == 0)
            {
                pinForward.Write(GpioPinValue.High);
                pinBackward.Write(GpioPinValue.Low);
                pinEnable.Write(GpioPinValue.High);
                Debug.WriteLine("Forward!");
            }
            else if ( i == 1  )
            {
                pinEnable.Write(GpioPinValue.Low);
                Debug.WriteLine("Slow Stop!");

            }
            else if ( i == 2 )
            {
                pinForward.Write(GpioPinValue.Low);
                pinBackward.Write(GpioPinValue.High);
                pinEnable.Write(GpioPinValue.High);
                Debug.WriteLine("Backward!");

            }
            else if (i == 3)
            {
                pinForward.Write(GpioPinValue.High);
                pinBackward.Write(GpioPinValue.High);
                Debug.WriteLine("FastStop!");

            }

            i++;
            if (i == 4)
                i = 0;
        }
    }
}
