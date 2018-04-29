using System;
using System.Collections.Generic;
using Vixen.Module.Script;
using Vixen.Sys;
using Vixen.Module.Effect;
using System.Linq;

namespace Vixen.User {
	public partial class TestVixenScript {
		
		protected override void Startup() {
			// Initialization goes here.
			Sequence.Length=TimeSpan.FromSeconds(25);
			
		     //Tree_Node is the name of a specific element in my setup, effect duration, effect start time, Level, Color
			Set_Level(All_Pixels, TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(5), 1, System.Drawing.Color.Blue);
			Set_Level(All_Pixels, TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(15), 1, System.Drawing.Color.Red);
			Notify("Set Level inserted");
		}
		
		protected override void Play() {
			// Code starts here.
			Notify("Starting Sequence");
			
		}
		
		protected override void Shutdown() {
			// Tear-down of initialization goes here.
		}

	}
}
