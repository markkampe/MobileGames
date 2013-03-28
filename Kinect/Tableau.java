import java.io.FileNotFoundException;
import java.io.InvalidObjectException;

import processing.core.PVector;
import processing.xml.*;

/** 
 * a collection of named objects, at known REAL-WORLD locations)
 * supporting the ability to find the object closest to a particular
 * set of REAL-WORLD coordinates.
 * 
 * A tableau can be created manually, based on received Kinect 
 * coordinates, or automatically from an XML file describing
 * names and (real-world) locations.  The chosen format (XML) 
 * can be fussy, but it is an extensible standard, supported by 
 * lots of tools and extensible ... which makes up for a lot.
 * 
 * Once a tableau has been created the Tableau.closest method can
 * be used to figure out whether or not a player has gotten within
 * touching distance of any of the objects.
 */
public class Tableau {
	
	private int num_objects;	//* number of objects currently in tableau
	private int max_objects;	//* maximum number of objects allowed in tableau
	private String names[];		//* names of the objects
	private int radius[];		//* closeness radius
	private PVector loc[];		//* (real world) locations of the objects
	
	static final int DEFAULT_RADIUS = 300;	// (mm) = 1 foot
	
	/**
	 * create a new (empty) tableau, which the client will then
	 * populate with a succession of add() calls.
	 * 
	 * @param max	number of objects allowed in this tableau
	 */
	public Tableau( int max ) {
		names = new String[max];
		loc = new PVector[max];	
		radius = new int[max];
		max_objects = max;
		num_objects = 0;	
	}
	
	/**
	 * create a tableau from an XML description of the names and
	 * (real world) locations of all known objects.
	 * 
	 * @param xml	XML elment for the root
	 */
	public Tableau( XMLElement xml ) throws InvalidObjectException {
		max_objects = 0;
		num_objects = 0;
		
		// make sure we can access the file
		String n = xml.getName();
		if (!n.equals("tableau"))
			throw new InvalidObjectException("document is not a 'tableau'");
		
		// allocate a list for all the objects
		int numObj = xml.getChildCount();
		if (numObj < 1)
			throw new InvalidObjectException("contains no objects");
		names = new String[numObj];
		loc = new PVector[numObj];
		radius = new int[numObj];
		max_objects = numObj;

		// initialize the situated objects as described in the tableau
		for( int i = 0; i < numObj; i++) {
			// make sure this is an object
			XMLElement c = xml.getChild(i);
			if (!c.getName().equals("object"))
				throw new InvalidObjectException("child[" + i + "] not an 'object'");
			
			n = c.getAttribute("name");
			if (n == null)
				throw new InvalidObjectException("child[" + i + "] has no 'name'");
			names[num_objects] = n;
			
			int r = c.getIntAttribute("radius");
			radius[num_objects] = (r > 0) ? r : DEFAULT_RADIUS;
	
			XMLElement p = c.getChild("position");
			if (p == null)
				throw new InvalidObjectException("child[" + i + "] has no 'position'");
			int x = p.getIntAttribute("x");
			int y = p.getIntAttribute("y");
			int z = p.getIntAttribute("z");
			PVector v = new PVector(x,y,z);	// assume these to be real-world coordinates
			loc[num_objects++] = v;
			// DEBUG
			System.out.println("found object " + n + " at <" + x + "," + y + "," + z + ">");
		}
	}
	
	/**
	 * add a new object to the tableau (w/kinect coordinates)
	 * 
	 * WARNING: these are kinect (not real world) coordinates!
	 * 
	 * @param name	name of the object to be added
	 * @param coords	real-world coordinates of object
	 * @param near	mm that count as abeing close
	 * 
	 * @return		success/failure
	 */
	public Boolean add(String name, PVector coords, int near) {
		if (num_objects >= max_objects)
			return false;
		
		names[num_objects] = name;
		radius[num_objects] = near;
		loc[num_objects++] = coords;
		return true;
	}
	
	/**
	 * remove an object from the tableau
	 * 
	 * @param name	name of the object to be removed
	 * @return	success/failure
	 */
	public Boolean remove(String name) {	
		
		for (int i = 0; i < num_objects; i++ ) {
			if (names[i].equals(name)) {
				while( i < num_objects - 1) {
					names[i] = names[i+1];
					loc[i] = loc[i+1];
					i++;
				}
				loc[i] = null;
				names[i] = "INVALID";
				num_objects--;
				return true;
			}
		}
		return false;
	}
	
	/**
	 * return the (real world) coordinates of a named object
	 * 
	 * @param name	name of the object to be found
	 * @return	(real world) location of desired object
	 * 		or null if no such object can be found
	 */
	public PVector find(String name) {
		for( int i = 0; i < num_objects; i++ ) {
			if (names[i].equals(name))
				return loc[i];
		}
		
		return null;
	}
	
	/**
	 * find the nearest "touchable" object in the tableau
	 * 
	 * @param p		point of interest (in real-world coordinates)
	 * 
	 * @return	name of nearest object in the tableau
	 */
	public String closest(PVector p) {
		
		// run through the tableau and find the closest object
		String closest = null;
		int distance = 1000*1000;	// a kilometer away
		for( int i = 0; i < num_objects; i++ ) {
			int d = (int) loc[i].dist(p);
			if (d <= radius[i] && d < distance) {
				distance = d;
				closest = names[i];
			}
		}
		
		return closest;
	}
	
	/**
	 * find the object in the tableau closest to specified  point
	 * (first, converting that to a real-world coordinate)
	 * 
	 * @param cx	kinect reported X coordinate
	 * @param cy	kinect reported Y coordinate
	 * @param cz	kinect reported Z coordinate
	 * 
	 * @return	name of nearest object in the tableau
	 * 			(check the distance, the nearest object may still be far)
	 */
	public String closest(int cx, int cy, int cz) {
		return this.closest(new KinectCoords(cx, cy, cz));
	}
}