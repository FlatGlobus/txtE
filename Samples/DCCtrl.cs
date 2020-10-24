using SkiaSharp;
using SkiaSharp.Views.Forms;
using Xamarin.Forms;

namespace DC
{
    public class DCCtrl : SKCanvasView
    {
        //[prop FrameColor Color Color.Black]
	public static readonly BindableProperty FrameColorProperty = 
		BindableProperty.Create(nameof(FrameColor), typeof(FrameColor), typeof(), Color.Black, propertyChanged: OnPropertyChanged);
	public Color FrameColor
	{
		get => (Color)GetValue(FrameColorProperty);
		set => SetValue(FrameColorProperty, value);
	}

        //[prop BackgroundColor Color Color.White]
	public static readonly BindableProperty BackgroundColorProperty = 
		BindableProperty.Create(nameof(BackgroundColor), typeof(BackgroundColor), typeof(), Color.White, propertyChanged: OnPropertyChanged);
	public Color BackgroundColor
	{
		get => (Color)GetValue(BackgroundColorProperty);
		set => SetValue(BackgroundColorProperty, value);
	}

	private static void OnPropertyChanged(BindableObject bindable, object oldVal, object newVal)
	{
		var bar = bindable as GradientBar;
		bar?.InvalidateSurface();
	}

    }
}
