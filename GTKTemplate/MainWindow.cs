//using Gdk;
using Gtk;
using System;
using System.Runtime.InteropServices;
using System.Windows.Forms.Integration;
using UI = Gtk.Builder.ObjectAttribute;


namespace GTKTemplate
{
    internal class MainWindow : Window
    {
        [DllImport("GLRender", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DrawMap();

        [UI] private Gtk.Label _label1 = null;
        [UI] private Gtk.Button _button1 = null;
        [UI] private Box MainBox = null;

        private int _counter;
        private bool isRealized = false;


        public MainWindow() : this(new Builder("MainWindow.glade")) { }

        private MainWindow(Builder builder) : base(builder.GetRawOwnedObject("MainWindow"))
        {
            builder.Autoconnect(this);

            DeleteEvent += Window_DeleteEvent;
            _button1.Clicked += Button1_Clicked;
            //WebView webView = new WebView();

            ElementHost host = new ElementHost();
            //host.Dock = DockStyle.Fill;
            //host.Child = new Microsoft.Web.WebView2.WinForms.WebView2(); // Use the WinForms Form with WebView2


            //Button btnStart = new Button("Start");
            //MainBox.PackEnd(host, true, true, 0);

            GLArea gLArea = new GLArea();
            MainBox.PackEnd(gLArea, true, true, 0);
            gLArea.Realized += GLArea_Realized;
            gLArea.AutoRender = true;
            gLArea.CreateContext += GLArea_CreateContext;
            gLArea.Render += OnRender;
            gLArea.Realize();



            MainBox.ShowAll();
        }

        private void GLArea_CreateContext(object o, CreateContextArgs args)
        {
            var context = (Gdk.GLContext)o;
            Console.WriteLine("GLArea_CreateContext");
            //context.MakeCurrent(glarea.Window);
            //glarea.MakeCurrent();
        }


        private void OnRender(object sender, RenderArgs args)
        {
            GLArea gLArea = sender as GLArea;

            if (gLArea == null || isRealized == false)
            {
                return;
            }
            DrawMap();

            gLArea.QueueRender();
        }

        private void GLArea_Realized(object sender, EventArgs e)
        {
            var glarea = sender as GLArea;
            glarea.MakeCurrent();
            glarea.HasAlpha= true;
            //glarea.HasDepthBuffer = true;
            glarea.HasStencilBuffer = true;
            glarea.UseEs = false;
            glarea.AutoRender = true;
            isRealized = true;
        }

        private void Window_DeleteEvent(object sender, DeleteEventArgs a)
        {
            Gtk.Application.Quit();
        }

        private void Button1_Clicked(object sender, EventArgs a)
        {
            _counter++;
            _label1.Text = "Hello World! This button has been clicked " + _counter + " time(s).";
        }
    }
}
