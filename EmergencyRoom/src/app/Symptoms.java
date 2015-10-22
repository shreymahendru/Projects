package app;

/**
 * 
 * Organizes the symptoms of a patient by the date they were noted.
 */
public class Symptoms extends DateRelated {
	
	/**
	 * Creates a new Symptoms map with date and a description of the patient's symptoms.	
	 * @param date - The date the symptoms were noted
	 * @param entry - A description of the patient's symptoms
	 */
	public Symptoms(String date, String entry){
		
		super(date, entry);
		
	}

}