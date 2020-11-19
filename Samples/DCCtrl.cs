using SkiaSharp;
using SkiaSharp.Views.Forms;
using Xamarin.Forms;

namespace DC
{
    public class DCCtrl : SKCanvasView
    {
        //[prop FrameColor Color Color.Black]
        //[prop BackgroundColor Color Color.White]

	private static void OnPropertyChanged(BindableObject bindable, object oldVal, object newVal)
	{
		var bar = bindable as GradientBar;
		bar?.InvalidateSurface();
	}
    }
}
