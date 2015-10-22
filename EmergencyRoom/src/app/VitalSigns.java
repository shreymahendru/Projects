package app;

/**
 * Organizes all of the Vital Signs for a given patient by date.
 */
public class VitalSigns extends DateRelated {
	
	private Double temperature;
	private Double heartRate;
	private String bloodPressure;
		
	
	/**
	 * Creates a new VitalSigns map with the initial temperature, heart rate and blood pressure.
	 * @param temperature - the patient's temperature
	 * @param heartRate - the patient's heart rate
	 * @param bloodPressure - the patient's blood pressure
	 */
	public VitalSigns(String date, Double temperature, Double heartRate, String bloodPressure){
		
		super(date, "Temperature: " + temperature + ", Heart Rate: " +
				heartRate + ", Blood Pressure: " + bloodPressure);
		
		this.date = date;
		this.temperature = temperature;
		this.heartRate = heartRate;
		this.bloodPressure = bloodPressure;
	}
	
	public void addEntry(String date, Double temp, Double heartRate, String bloodPressure){
		
		String entry = "Temperature: " + temp + ", Heart Rate: " +
				heartRate + ", Blood Pressure: " + bloodPressure;
		
		this.dateToList.put(date, entry);
		
		//Update these instance variables
		setTemperature(temp);
		setHeartRate(heartRate);
		setBloodPressure(bloodPressure);
	}

	public Double getTemperature() {
		return temperature;
	}

	public void setTemperature(Double temperature) {
		this.temperature = temperature;
	}

	public Double getHeartRate() {
		return heartRate;
	}

	public void setHeartRate(Double heartRate) {
		this.heartRate = heartRate;
	}

	public String getBloodPressure() {
		return bloodPressure;
	}

	public void setBloodPressure(String bloodPressure) {
		this.bloodPressure = bloodPressure;
	}
	
	

}