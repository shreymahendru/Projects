
public String niceTip(String str)
    {
        str  = str.substring(1);
        double amt;
        try {
            amt = Double.parseDouble(str);  // covert amount to double throws exception if not a number.
        }
        catch (NumberFormatException e)
        {
            return "error";
        }

        double ten= .01*amt+amt;
        double twenty= .02*amt+amt;

        int decimalValue = Integer.parseInt(str.substring(str.indexOf('.')+1));
        double diff = (100-decimalValue); //to make whole
        double whole = (amt+(diff/100));
        while (whole < ten) //if less than 10%
        {
            whole +=1;
        }
        if (whole > twenty)
        {
            return "error";
        }
        else
        {
            return "$"+whole+"0";
        }

    }