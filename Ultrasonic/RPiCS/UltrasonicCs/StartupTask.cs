using System;
using Windows.ApplicationModel.Background;
using Windows.Devices.Gpio;
using System.Diagnostics;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace UltrasonicCs
{
    /// <summary>
    /// Get the distance from Utltrasonic sensor, lighting up LEDs if the change in distance
    /// exceeds a threshold
    /// 
    /// Basic logic ported from http://www.raspberrypi-spy.co.uk/2012/12/ultrasonic-distance-measurement-using-python-part-1/
    /// </summary>
    public sealed class StartupTask : IBackgroundTask
    {
        const int GPIO_TRIGGER = 5;
        const int GPIO_ECHO = 6;
        const int GPIO_RED = 26;
        const int GPIO_BLUE = 13;
        const int GPIO_GREEN = 19;
        const int GPIO_BEEPER = 21;

        GpioPin _trigger;
        GpioPin _echo;

        const int _averageCount = 3;    // how many distance reading to average
        const int _sleepMs = 100;

        const double _threshold1 = 25;  // moving 25/cm/(.5)sec (approx) turn on green light
        const double _threshold2 = 100; // moving in faster, turn on red light

        Stopwatch stopwatch = Stopwatch.StartNew();

        public void Run(IBackgroundTaskInstance taskInstance)
        {
            // setup the Gpio
            GpioController controller = GpioController.GetDefault();

            _trigger = controller.OpenPin(GPIO_TRIGGER);
            _trigger.SetDriveMode(GpioPinDriveMode.Output);

            _echo = controller.OpenPin(GPIO_ECHO);
            _echo.SetDriveMode(GpioPinDriveMode.Input);

            var beeper = controller.OpenPin(GPIO_BEEPER);
            beeper.SetDriveMode(GpioPinDriveMode.Output);

            var red = controller.OpenPin(GPIO_RED);
            red.SetDriveMode(GpioPinDriveMode.Output);
            var green = controller.OpenPin(GPIO_GREEN);
            green.SetDriveMode(GpioPinDriveMode.Output);
            var blue = controller.OpenPin(GPIO_BLUE);
            blue.SetDriveMode(GpioPinDriveMode.Output);

            _trigger.Write(GpioPinValue.Low);
            wait(500); // let it settle

            blue.Write(GpioPinValue.High); // blue light is always on when running

            double prevDistance = getAverageDistance(_averageCount, _sleepMs);
            var beeperOn = false;
            while (true)
            {
                double currentDistance = getAverageDistance(_averageCount, _sleepMs);
                double change = Math.Abs(currentDistance - prevDistance);
                Debug.WriteLine("Change: {0:0.0}cm", change);

                if (change < _threshold1)
                {
                    green.Write(GpioPinValue.Low);
                    red.Write(GpioPinValue.Low);
                    if (beeperOn)
                    {
                        beeper.Write(GpioPinValue.Low);
                        beeperOn = false;
                    }
                }
                else if (change > _threshold2)
                {
                    beeper.Write(GpioPinValue.High);
                    beeperOn = true;
                    green.Write(GpioPinValue.High);
                    red.Write(GpioPinValue.High);
                }
                else 
                {
                    green.Write(GpioPinValue.High);
                }
                prevDistance = currentDistance;

                wait(250);

            }
        }

        // get average distance readings, sleeping sleepMs between readings
        private double getAverageDistance( int count, int sleepMs )
        {

            double total = 0;

            for ( int i = 0; i < count; i++ )
            {
                total += getDistance();
                if ( i < count-1 )
                    wait(sleepMs);
            }
            return total / count;
        }

        /// <summary>
        /// get the distance, pulsing the sensor
        /// </summary>
        /// <returns>the distance or -1 if timed out waiting for response</returns>
        private double getDistance()
        {
            // Send 10us pulse to trigger
            _trigger.Write(GpioPinValue.High);
            wait(.01);
            _trigger.Write(GpioPinValue.Low);

            // now wait for echo pulse, checking for timeouts so it doesn't get stuck
            var start = stopwatch.ElapsedTicks;
            long stop = 0;
            long tempStop = 0;
            long tempStart = stopwatch.ElapsedMilliseconds;

            while (_echo.Read() == GpioPinValue.Low)
            {
                start = stopwatch.ElapsedTicks;
                tempStop = stopwatch.ElapsedMilliseconds;
                if (tempStop - tempStart > 500)
                    break;
            }
            if (tempStop - tempStart > 500)
            {
                System.Diagnostics.Debug.WriteLine("Timed out in first wait");
                return -1;
            }

            tempStart = stopwatch.ElapsedMilliseconds;
            while (_echo.Read() == GpioPinValue.High)
            {
                stop = stopwatch.ElapsedTicks;
                tempStop = stopwatch.ElapsedMilliseconds;
                if (tempStop - stop > 500)
                    break;
            }
            if (tempStop - stop > 500)
            {
                System.Diagnostics.Debug.WriteLine("Timed out in second wait");
                return -1;
            }

            // Calculate pulse length
            double elapsed = (stop - (double)start) / Stopwatch.Frequency;

            // Distance pulse travelled in that time is time
            // multiplied by the speed of sound (cm/s) cut 
            // in half since time is round trip
            var distance = elapsed * (34000 / 2);

            return distance;
        }

        // From MS example
        //A synchronous wait is used to avoid yielding the thread 
        //This method calculates the number of CPU ticks will elapse in the specified time and spins
        //in a loop until that threshold is hit. This allows for very precise timing.
        private void wait(double milliseconds)
        {
            long initialTick = stopwatch.ElapsedTicks;
            long initialElapsed = stopwatch.ElapsedMilliseconds;
            double desiredTicks = milliseconds / 1000.0 * Stopwatch.Frequency;
            double finalTick = initialTick + desiredTicks;
            while (stopwatch.ElapsedTicks < finalTick)
            {

            }
        }
    }
}
