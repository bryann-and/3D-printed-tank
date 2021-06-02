using SharpDX.XInput;
using System;
using System.Drawing;

namespace ControleTanqueServidor
{
    public class XInputController
    {
        Controller controller;
        Gamepad gamepad;
        public bool connected = false;
        public int deadband = 4000;
        public Point leftThumb, rightThumb = new Point(0, 0);
        public float leftTrigger, rightTrigger;

        public XInputController()
        {
            controller = new Controller(UserIndex.One);
            connected = controller.IsConnected;
        }

        // Call this method to update all class values
        public void Update()
        {
            if (!connected)
            {
                leftThumb = rightThumb = new Point(0, 0);
                return;
            }        
            
            gamepad = controller.GetState().Gamepad;
            
            leftThumb.X = (Math.Abs((int) gamepad.LeftThumbX) < deadband) ? 0 : gamepad.LeftThumbX;
            leftThumb.Y = (Math.Abs((int) gamepad.LeftThumbY) < deadband) ? 0 : gamepad.LeftThumbY;
            rightThumb.X = (Math.Abs((int) gamepad.RightThumbX) < deadband) ? 0 : gamepad.RightThumbX;
            rightThumb.Y = (Math.Abs((int) gamepad.RightThumbY) < deadband) ? 0 : gamepad.RightThumbY;

            leftTrigger = gamepad.LeftTrigger;
            rightTrigger = gamepad.RightTrigger;
        }
    }
}
