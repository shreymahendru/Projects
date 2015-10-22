package app;

import java.io.*;
import java.util.HashMap;
import java.util.Map;


public class Nurse {
	/**
	 * If a new patient is added then it updates it's info(health card no. , name , Date of birth) in the text file patients.txt.
	 * if there is no such file i.e no patients in the ER then it creates the file a new file named patients.txt
	 * and adds the patients info to it.
	 * @param p
	 * @throws IOException
	 */
	
	public static void set_basic_info(Map<String, String> p)throws IOException
	{
		String name=p.get("Name");             //getting the patient's name, health card no and DOB  
		String CardNo= p.get("Health Card #");//from the map given to the function.
		String DOB=p.get("DOB");
		
		String all= CardNo+','+name+','+DOB+'\n';// adding everything into a String
		
		File myFile = new File("patient.txt"); // opening the file 
		
		if (myFile.exists()) //check if it's there
		{
			 try
			   {
			       FileOutputStream fOut = new FileOutputStream(myFile);
			       OutputStreamWriter myOutWriter = new OutputStreamWriter(fOut);
			       myOutWriter.append(all); //writing to the file
			       myOutWriter.close();
			       fOut.close();
			    } 
			 catch(Exception e){
				 System.out.println("Error reading patient from file.");
				 }
		
		}
		else
		 {
			 myFile.createNewFile(); // create a new patients.txt if it does not exist.
			 set_basic_info(p);// adding first patient's info to it 
		 }
		}
	
	/**
	 *  reads basic info of patient such as health card, name, date of birth from the file patients.txt
	 * @param healthcardno
	 * @return basic info  (a Map with key/value as strings)
	 */

	public static Map<String, String> get_basic_info(String healthcardno)
	{
		String ret= "";
		File myFile = new File("patient.txt");
		try{
			BufferedReader br = new BufferedReader(new FileReader(myFile));
			String line=br.readLine();
			while (line != null) 
			{
				String[] parts= line.split(",");
				if (parts[0]== healthcardno) // checking for the health card no given to find it's info
				{
					ret=line;
					break;
				}
				
			line=br.readLine();			
			}
			br.close();
		}
		
		catch(IOException e)
		{
			 System.out.println("Error reading patient from file given health card.");
		}
		
		Map<String,String> basic_info=new HashMap<String, String>();
		String[] s = ret.split(",");
		basic_info.put("Health Card #",s[0]);
		basic_info.put("Name",s[1]);
		basic_info.put("DOB",s[2]);
		return basic_info;
		}
	
	/**
	 *  add the vital signs and symptoms recorded for a given date to a patient's file. 
	 *  if it is a new patient then creates a new file with the name of the is the health card no
	 *  and then add the data.
	 * @param card
	 * @param date
	 * @param vitalSigns
	 * @param Symptoms
	 * @throws IOException
	 */
	public static void set_signs_symptoms(String card, String date, VitalSigns vitalSigns, Symptoms Symptoms) throws IOException
	{
	String name=card+".txt";
	String vital = vitalSigns.getTemperature()+","+vitalSigns.getBloodPressure()+","+vitalSigns.getHeartRate();// changing vital signs to string to store it in a file
	String symptoms = Symptoms.entry;
	String s= date+";"+vital+";"+symptoms;//adding health card no, vital signs and symptoms in a single string to write in a file.
    File myFile = new File(name);
		
		if (myFile.exists())
		{
			
			try
			   {
			       FileOutputStream fOut = new FileOutputStream(myFile);
			       OutputStreamWriter myOutWriter = new OutputStreamWriter(fOut);
			       
			       myOutWriter.append(s);
			       myOutWriter.close();
			       fOut.close();
			    } 
			 catch(Exception e)
			    {
				 System.out.println("Error reading patient from file given health card.");
			    }
		
		}
		else
		 {
			 myFile.createNewFile();// create a new file if it's a new patient.
			 set_signs_symptoms(card, date, vitalSigns, Symptoms);// add vital signs and symptoms to it 
		 }
			
	}
	
	
	/**
	 *  read vital signs and symptoms of specific patient's file at a given date
	 *  initialize class vitalsigns and symptoms to store the value from the file and
	 *  a map of the objects of the class in a map 
	 * @param Cardno
	 * @param date
	 * @return p (Map<String, Object>)
	 */
	public Map<String, Object> get_signs_symptoms(String Cardno,String date)
	{
		String filename= Cardno+".txt";
		File myfile= new File(filename);
		String ret="";
		try{
			BufferedReader br = new BufferedReader(new FileReader(myfile));
			String line=br.readLine();
			while (line != null)
			{
				String[] parts= line.split(",");
				if (parts[0] == date)
				{
					ret=line;
					break;
				}
				
			line=br.readLine();			
			}
			br.close();
		}
		catch(IOException e)
		{
			 System.out.println("Error reading patient from file given health card.");
		}
		
		Map<String, Object> p = new HashMap<String,Object>();
		String[] parts2 = ret.split(";");
		String vitalsigns = parts2[1];
		String[] comp = vitalsigns.split(",");
		
		// comp[0] = temp, comp[2] = heartRate, comp[1] = bloodPressure
		VitalSigns vitalSigns = new VitalSigns(date, Double.parseDouble(comp[0]), Double.parseDouble(comp[2]), comp[1]);
		Symptoms symptoms = new Symptoms(date, parts2[3]);
		p.put("VitalSigns", vitalSigns);
		p.put("Symptoms", symptoms);
		return p;
	}
}