import java.io.FileNotFoundException;
import java.io.InvalidObjectException;

import processing.core.*;
import processing.xml.XMLElement;


/*
 * Test program for the Tableau management package
 * 
 * Locations are translated from raw Kinect PVectors into real-world
 * coordinates via the KinectCoords class.  This translation enables
 * meaningful 3D distance computations.
 * 
 * A tableau of objects is instantiated from an XML file describing 
 * the names and (real-world) locations of the interesting objects.
 * That file can be in the program directory or at a known URL.
 * 
 * This test program treats mouse clicks as players at a the
 * specified (kinect) location.  The Tableau.closest() method is used 
 * to determine which situated object (if any) a player has touched.
 * If one is selected, the object name is used to form the name of
 * a file that is read and displayed.  This is meant to demonstrate
 * both proximity detection and fetching data from the web (or file).
 * 
 * I left in a bunch of DEBUG stuff to help you figure out what is 
 * going on, but they should be commented out.
 */
public class MyProcessingSketch extends PApplet {
	
	Tableau t;		// the tableau we are working with
	String base;	// where we find our description files
	
	private static final String URL_PREFIX = "http://www.cs.pomona.edu/~markk/tableau/";
	private static final String BASE_PREFIX = "";
	private static final String TABLEAU_FILE = "Tableau.xml";
	private static final String OBJECT_SUFFIX = ".txt";
	
	private static final int TOUCHING_RANGE = 100;	// mm ~ 4 inches
	
	
	@SuppressWarnings("unused")
	public void setup() {
		size(640,480);
		background(0);
		
		base = URL_PREFIX;			// use URL
		// base = FILE_PREFIX;		// use local files
		// base = null;				// to run the default test
		String tableau = base + TABLEAU_FILE;
		
		if (tableau == null) {
			// spread test objects around the playing field
			System.out.println("Using hard-coded test tableau");
			t = new Tableau(10);
			t.add("CCM", new KinectCoords(320,240,1000), TOUCHING_RANGE);
			t.add("CCF", new KinectCoords(320,240, 900), TOUCHING_RANGE);
			t.add("LCM", new KinectCoords(160,240,1000), TOUCHING_RANGE);
			t.add("RCM", new KinectCoords(480,240,1000), TOUCHING_RANGE);
			t.add("CTM", new KinectCoords(320,120,1000), TOUCHING_RANGE);
			t.add("CBM", new KinectCoords(320,360,1000), TOUCHING_RANGE);
			t.add("TLM", new KinectCoords(160,120,1000), TOUCHING_RANGE);
			t.add("TRM", new KinectCoords(480,120,1000), TOUCHING_RANGE);
			t.add("BLM", new KinectCoords(160,360,1000), TOUCHING_RANGE);
			t.add("BRM", new KinectCoords(480,360,1000), TOUCHING_RANGE);
		} else { // reading the input from a specified file or URL
			try {
				System.out.println("Using tableau from " + tableau);
				XMLElement x = new XMLElement(this, tableau);
				t = new Tableau( x );
			} catch (InvalidObjectException e) {
				System.out.println( e + ": " + tableau);
			}
		}
	}
	
	// doesn't do much in this test program
	public void draw() {
	}

	/**
	 * trigger an event associated with a named object
	 * 
	 * @param objname	object we have just approached
	 */
	private void play( String objname ) {
		String objfile = base + objname + OBJECT_SUFFIX;
		
		String lines[] = loadStrings(objfile);
		if (lines == null) {
			System.out.println("ERROR: unable to read play file " + objfile);
			return;
		}
		
		for( int i = 0; i < lines.length; i++ ) {
			System.out.println( lines[i] );
		}
	}
	
	// interpret mouse click as 640x480 coordinates z=1000
	// the Override is because Processing doesn't want me to do this
	@Override public void mouseReleased() {
		int ONE_METER = 770;	// (in kinect coordinates, 1000/1.3)
		KinectCoords p = new KinectCoords(mouseX, mouseY, ONE_METER);
		// DEBUG
		System.out.println("mouse<" + mouseX + "," + mouseY + "," + ONE_METER + "> -> real<"
				+ (int) p.x + "," + (int) p.y + "," + (int) p.z + ">");
    	String o = t.closest(p);
    	if (o != null) {
    		PVector objloc = t.find(o);
    		int d = (int) objloc.dist(p);
    		// DEBUG
    	    System.out.println("\n<" + mouseX + "," + mouseY + "," + ONE_METER + "> is "
        				+ d + "mm from " + o + "\n");
    	    play( o );
    	} else {
    		// DEBUG
    	    System.out.println("\nNOJOY for <" + (int) p.x + "," + (int) p.y + "," + (int) p.z + ">\n");
    	}
	}
}