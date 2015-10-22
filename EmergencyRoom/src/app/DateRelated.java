package app;

import java.util.TreeMap;

/**
 * Implements a basic "entry by date" map
 */
public abstract class DateRelated {
	
	protected TreeMap<String, String> dateToList;
	protected String date;
	protected String entry;
	
	/**
	 * Creates a new date related map with an initial entry.
	 * 
	 * @param date - The date of the entry
	 * @param entry - The entry to be added
	 */
	public DateRelated(String date, String entry){
		
		this.dateToList = new TreeMap<String, String>();
		this.date = date;
		this.entry = entry;
		
		this.dateToList.put(this.date, this.entry);
	}
	
	/**
	 * Adds a new date entry to the map.
	 * @param date
	 *           - The date of the entry
	 * @param entry
	 *           - The entry to be added
	 */
	public void addEntry(String date, String entry){
		this.dateToList.put(date, entry);
	}
	
	
	/**
	 * Returns the most recent entry in the map.
	 * 
	 * @return - The most recent entry
	 */
	public String recentEntry(){
		
		return this.dateToList.get(this.dateToList.firstKey());
	}
	
	
	/**
	 * Returns the entry from a specified date
	 * 
	 * @param date
	 *           - The date for the desired entry
	 * @return - The entry for the specified date
	 */
	public String reviewEntry(String date){
		return this.dateToList.get(date);
	}

}