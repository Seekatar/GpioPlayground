using System;
using System.IO.Ports;
using System.Management;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Console;
using System.Drawing;

namespace UsbReadWrite
{
	class Program
	{
		const byte SHOW_COLOR = 1;
		const byte ALERT = 2;

		static void Main(string[] args)
		{
			bool sendHrMin = false;

			var portName = getArduinoPort();
			if (portName == null)
				portName = "COM7";

			if (portName != null)
			{
				WriteLine("Found Arduino on port {0}", portName);

				SerialPort port = new SerialPort(portName, 9600);
				port.Open();
				port.ReceivedBytesThreshold = 40;
				port.DataReceived += Port_DataReceived;

				if (sendHrMin)
					doHrMin(portName, port);
				else
					doColors(portName, port);
				port.Close();
			}
		}

		private static void doColors(string portName, SerialPort port)
		{
			WriteLine("Enter name of color, x to exit", portName);

			KnownColor kc;
			byte[] rgb = new byte[4];

			while (true)
			{
				var s = Console.ReadLine();
				if (s.Length < 1)
					continue;

				var names = Enum.GetNames(typeof(KnownColor));
				var color = names.SingleOrDefault(o => String.Equals(s, o, StringComparison.OrdinalIgnoreCase));
				if ( color != null && Enum.TryParse<KnownColor>(color, out kc) )
				{
					var c = Color.FromKnownColor(kc);
					rgb[0] = SHOW_COLOR; // show color command 
					rgb[1] = c.R;
					rgb[2] = c.G;
					rgb[3] = c.B;

					port.Write(rgb, 0, 4);
				}
				else if (String.Equals(s,"a",StringComparison.OrdinalIgnoreCase))
				{
					rgb[0] = ALERT; // show color command 

					port.Write(rgb, 0, 4);
				}
				else if (String.Equals(s, "x", StringComparison.OrdinalIgnoreCase))
				{
					Write("Exiting");
					break;
				}
				else
				{
					Write("Unknown command");
				}

			}
		}

		private static void doHrMin(string portName, SerialPort port)
		{
			WriteLine("Enter hr min, x to exit", portName);

			while (true)
			{
				byte[] hrmin = new byte[2];
				var s = Console.ReadLine().Split(" ".ToCharArray());
				if (s.Length > 1 && byte.TryParse(s[0], out hrmin[0]) && byte.TryParse(s[1], out hrmin[1]))
				{
					port.Write(hrmin, 0, 2);
				}
				else if (s[0].Equals("x"))
				{
					break;
				}

			}
		}

		private static void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			WriteLine("Got something!");
			var s = ((SerialPort)sender).ReadExisting();
			WriteLine(s);
		}

		static string getArduinoPort()
		{
			var connectionScope = new ManagementScope();
			var serialQuery = new SelectQuery("SELECT * FROM Win32_SerialPort");
			var searcher = new ManagementObjectSearcher(connectionScope, serialQuery);

			try
			{
				foreach (ManagementObject item in searcher.Get())
				{
					var desc = item["Description"].ToString();

					if (desc.Contains("Arduino"))
					{
						return item["DeviceID"].ToString(); 
					}
				}
			}
			catch (ManagementException e)
			{
				/* Do Nothing */
			}

			return null;
		}
	}
}
