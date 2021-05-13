


<?php 

/*
Name : Marcos Hernandez
Senior Project
*/

//connect to odbc for the server to work
include("connection.php");

//function calls the information entered in the form
function getData(){
    $data = array();
    $data[1] = $_POST['UserId'];
    $data[2] = $_POST['RSAddress'];
    $data[3] = $_POST['RCity'];
    $data[4] = $_POST['RState'];
    $data[5] = $_POST['RZipCode'];
    $data[6] = $_POST['RContactName'];
    $data[7] = $_POST['RContactPhone'];
    $data[8] = $_POST['RGFEIN'];
    $data[9] = $_POST['EmployerFilingBaseId'];
    $data[10] = $_POST['RGAddress'];
    $data[11] = $_POST['RGCity'];
    $data[12] = $_POST['RGState'];
    $data[13] = $_POST['RGZipCode'];
    $data[14] = $_POST['RGContactName'];
    $data[15] = $_POST['RGContactPhone'];
    $data[16] = $_POST['NumberForms18'];


    //In the database, some columns were create as a bit type, so for the code to work, I added a true or false code (0:1)
    $data[17] = ($_POST['AuthTrans19'] == '' )? 0:1 ;
    $data[18] = $_POST['ALENumberForms20'];
    $data[19] = ($_POST['ALEGroup21'] == '' )? 0:1 ;
    $data[20] = ($_POST['QOfferMethod22A'] == '' )? 0:1 ;
    $data[21] = ($_POST['QOfferTranRelief22B'] == '' )? 0:1 ;
    $data[22] = ($_POST['Section498022C'] == '' )? 0:1 ;
    $data[23] = ($_POST['Offer98Percent22D'] == '' )? 0:1 ;
    $data[24] = $_POST['RTitle'];
    $data[25] = ($_POST['AllEssential'] == '' )? 0:1 ;
    $data[26] = $_POST['AllFTCount'];
    $data[27] = $_POST['AllTotalCount'];
    $data[28] = ($_POST['All4980Indicator'] == '' )? 0:1 ;

    $data[29] = ($_POST['JanEssential'] == '' )? 0:1 ;
    $data[30] = $_POST['JanFTCount'];
    $data[31] = $_POST['JanTotalCount'];
    $data[32] = ($_POST['Jan4980Indicator'] == '' )? 0:1 ;

    $data[33] = ($_POST['FebEssential'] == '' )? 0:1 ;
    $data[34] = $_POST['FebFTCount'];
    $data[35] = $_POST['FebTotalCount'];
    $data[36] = ($_POST['Feb4980Indicator'] == '' )? 0:1 ;

    $data[37] = ($_POST['MarEssential'] == '' )? 0:1 ;
    $data[38] = $_POST['MarFTCount'];
    $data[39] = $_POST['MarTotalCount'];
    $data[40] = ($_POST['Mar4980Indicator'] == '' )? 0:1 ;

    $data[41] = ($_POST['AprEssential'] == '' )? 0:1 ;
    $data[42] = $_POST['AprFTCount'];
    $data[43] = $_POST['AprTotalCount'];
    $data[44] = ($_POST['Apr4980Indicator'] == '' )? 0:1 ;

    $data[45] = ($_POST['MayEssential'] == '' )? 0:1 ;
    $data[46] = $_POST['MayFTCount'];
    $data[47] = $_POST['MayTotalCount'];
    $data[48] = ($_POST['May4980Indicator'] == '' )? 0:1 ;

    $data[49] = ($_POST['JunEssential'] == '' )? 0:1 ;
    $data[50] = $_POST['JunFTCount'];
    $data[51] = $_POST['JunTotalCount'];
    $data[52] = ($_POST['Jun4980Indicator'] == '' )? 0:1 ;

    $data[53] = ($_POST['JulEssential'] == '' )? 0:1 ;
    $data[54] = $_POST['JulFTCount'];
    $data[55] = $_POST['JulTotalCount'];
    $data[56] = ($_POST['Jul4980Indicator'] == '' )? 0:1 ;

    $data[57] = ($_POST['AugEssential'] == '' )? 0:1 ;
    $data[58] = $_POST['AugFTCount'];
    $data[59] = $_POST['AugTotalCount'];
    $data[60] = ($_POST['Aug4980Indicator'] == '' )? 0:1 ;

    $data[61] = ($_POST['SepEssential'] == '' )? 0:1 ;
    $data[62] = $_POST['SepFTCount'];
    $data[63] = $_POST['SepTotalCount'];
    $data[64] = ($_POST['Sep4980Indicator'] == '' )? 0:1 ;

    $data[65] = ($_POST['OctEssential'] == '' )? 0:1 ;
    $data[66] = $_POST['OctFTCount'];
    $data[67] = $_POST['OctTotalCount'];
    $data[68] = ($_POST['Oct4980Indicator'] == '' )? 0:1 ;

    $data[69] = ($_POST['NovEssential'] == '' )? 0:1 ;
    $data[70] = $_POST['NovFTCount'];
    $data[71] = $_POST['NovTotalCount'];
    $data[72] = ($_POST['Nov4980Indicator'] == '' )? 0:1 ;

    $data[73] = ($_POST['DecEssential'] == '' )? 0:1 ;
    $data[74] = $_POST['DecFTCount'];
    $data[75] = $_POST['DecTotalCount'];
    $data[76] = ($_POST['Dec4980Indicator'] == '' )? 0:1 ;
    return $data;
}

//if statement reads the columns included in the table and saves the data entered
if(isset($_POST['insert'])){
  $info = getData();
  $insert = "INSERT INTO [YEBFLog] ([UserId], [RSAddress], [RCity], [RState], [RZipCode], [RContactName], [RContactPhone]
  , [RGFEIN], [EmployerFilingBaseId], [RGAddress], [RGCity], [RGState], [RGZipCode], [RGContactName], [RGContactPhone], [NumberForms18], [AuthTrans19]
  , [ALENumberForms20], [ALEGroup21], [QOfferMethod22A], [QOfferTranRelief22B], [Section498022C], [Offer98Percent22D], [RTitle]
  , [AllEssential], [AllFTCount], [AllTotalCount], [All4980Indicator]
  , [JanEssential], [JanFTCount], [JanTotalCount], [Jan4980Indicator]
  , [FebEssential], [FebFTCount], [FebTotalCount], [Feb4980Indicator]
  , [MarEssential], [MarFTCount], [MarTotalCount], [Mar4980Indicator]
  , [AprEssential], [AprFTCount], [AprTotalCount], [Apr4980Indicator]
  , [MayEssential], [MayFTCount], [MayTotalCount], [May4980Indicator]
  , [JunEssential], [JunFTCount], [JunTotalCount], [Jun4980Indicator]
  , [JulEssential], [JulFTCount], [JulTotalCount], [Jul4980Indicator]
  , [AugEssential], [AugFTCount], [AugTotalCount], [Aug4980Indicator]
  , [SepEssential], [SepFTCount], [SepTotalCount], [Sep4980Indicator]
  , [OctEssential], [OctFTCount], [OctTotalCount], [Oct4980Indicator]
  , [NovEssential], [NovFTCount], [NovTotalCount], [Nov4980Indicator]
  , [DecEssential], [DecFTCount], [DecTotalCount], [Dec4980Indicator]) 
  VALUES ('$info[1]', '$info[2]', '$info[3]', '$info[4]', '$info[5]', '$info[6]', '$info[7]'
  , '$info[8]', '$info[9]', '$info[10]', '$info[11]', '$info[12]', '$info[13]', '$info[14]', '$info[15]', '$info[16]', '$info[17]'
  , '$info[18]', '$info[19]', '$info[20]', '$info[21]', '$info[22]', '$info[23]', '$info[24]', '$info[25]', '$info[26]', '$info[27]', '$info[28]'
  , '$info[29]', '$info[30]', '$info[31]', '$info[32]'
  , '$info[33]', '$info[34]', '$info[35]', '$info[36]'
  , '$info[37]', '$info[38]', '$info[39]', '$info[40]'
  , '$info[41]', '$info[42]', '$info[43]', '$info[44]'
  , '$info[45]', '$info[46]', '$info[47]', '$info[48]'
  , '$info[49]', '$info[50]', '$info[51]', '$info[52]'
  , '$info[53]', '$info[54]', '$info[55]', '$info[56]'
  , '$info[57]', '$info[58]', '$info[59]', '$info[60]'
  , '$info[61]', '$info[62]', '$info[63]', '$info[64]'
  , '$info[65]', '$info[66]', '$info[67]', '$info[68]'
  , '$info[69]', '$info[70]', '$info[71]', '$info[72]'
  , '$info[73]', '$info[74]', '$info[75]', '$info[76]')";

  //sends data to the database
  $result = odbc_exec($connect, $insert);
  //print_r($info);
}


$EmployerArray = array();

$query = "SELECT * FROM YEEmployerBaseFiled";
$resultQuery = odbc_exec($connect, $query);
while($item = odbc_fetch_object($resultQuery)){
  $EmployerArray[] = array('ID' => $item->EmployerFilingBaseId, 'Name' => $item->REName);
};

function getOtherData(){
  $data = array();
  $data[1] = $_POST['OtherName'];
  $data[2] = $_POST['OtherFEIN'];
  return $data;

}


if(isset($_POST['submit'])){
$info = getOtherData();
$insert3 = "INSERT INTO [YE1094OtherALE] ([OtherName], [OtherFEIN]) 
VALUES ('$info[1]', '$info[2]')";

$result = odbc_exec($connect, $insert3);
//print_r($info);
}


?>


<!DOCTYPE html>
<html lang="en">
<head>
  <title>Senior Project</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
  
  <style>
    table{
      border-collapse: collapse;
      width: 100%;
    }

    th, tr{
      border: 2px solid black;
    }

    .btn-group{
      text-align: center;
    }
  </style>
</head>
<body>

<div class="jumbotron text-center">
  <h1>1094C Form</h1>
  <a href="index.php"><button>Return</button></a>
</div>

<table>
<div class="container">
<form method = "POST">


<!--

  The html is saved in a form method. It includes all parts from the 1094C form
  ID and name helps the program to know what its looking for and what data type will be saved in the database

-->
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part I: Applicable Large Employer Member (ALE Member)</h2>
      <th>
      <label>1. Name of ALE Member (Employer)</label>
      <select class="form-control" name="EmpID">
        <option>
          Select Employer
        </option>
        <?php foreach ($EmployerArray as $value) {?>
            <option value="<?php echo $value['ID']?>"> <?php echo $value['Name']?> </option>
        
        
        <?php }?>

      </select>
      </th>
      <th>
      <label for="usr">2. Employer identification number (EIN)</label>
        <input type="text" class="form-control" id="UserId" name="UserId">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">3. Street address (including room or suite no.): </label>
        <input type="text" class="form-control" id="RSAddress" name="RSAddress">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">4. City or town</label>
        <input type="text" class="form-control" id="RCity" name="RCity">
        </th>

      <div class="form-group">
      <th colspan="2">
      <label for="usr">5. State or province </label>
        <input type="text" class="form-control" id="RState" name="RState">
        </th>
        </tr>
      </div>

    <tr>
    <th>
      <div class="form-group">
      <label for="usr">6. Country and ZIP or foreign postal code </label>
        <input type="text" class="form-control" id="RZipCode" name="RZipCode">
      </div>
    </th>

    <th>
        <div class="form-group">
        <label for="usr">7. Name of person to contact</label>
          <input type="text" class="form-control" id="RContactName" name="RContactName">
        </div>
      </th>

      <th>
        <div class="form-group" >
        <label for="usr">8. Contact telephone number </label>
          <input type="text" class="form-control" id="RContactPhone" name="RContactPhone">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">9. Name of Designated Government Entity (only if applicable) </label>
          <input type="text" class="form-control" id="RGFEIN" name="RGFEIN">
        </div>
      </th>

      <th colspan="2">
        <div class="form-group">
        <label for="usr">10. Employer identification number (EIN) </label>
          <input type="text" class="form-control" id="EmployerFilingBaseId" name="EmployerFilingBaseId">
        </div>
      </th>
      </tr>

<tr>
      <th>
        <div class="form-group">
        <label for="usr">11. Street address (including room or suite no.) </label>
          <input type="text" class="form-control" id="RGAddress" name="RGAddress">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">12. City or town </label>
          <input type="text" class="form-control" id="RGCity" name="RGCity">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">13. State or province </label>
          <input type="text" class="form-control" id="RGState" name="RGState">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">14. Country and ZIP or foreign postal code </label>
          <input type="text" class="form-control" id="RGZipCode" name="RGZipCode">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">15. Name of person to contact </label>
          <input type="text" class="form-control" id="RGContactName" name="RGContactName">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">16. Contact telephone number </label>
          <input type="text" class="form-control" id="RGContactPhone" name="RGContactPhone">
        </div>
      </th>
      </tr>
    <tr>

    <th colspan="2">
        <div class="form-group">
        <label for="usr">17. Reserved......................................... </label>
          <input type="checkbox" class="form-control">
        </div>
      </th>

      <th colspan="2">
        <div class="form-group">
        <label for="usr">18 Total number of Forms 1095-C submitted with this transmittal . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ▶</label>
          <input type="text" class="form-control" id="NumberForms18" name="NumberForms18">
        </div>
      </th>

      <th colspan="6">
        <div class="form-group">
        <label for="usr">19 Is this the authoritative transmittal for this ALE Member? If “Yes,” check the box and continue. If “No,” see instructions 
 </label>
          <input type="checkbox" class="form-control" id="AuthTrans19" name="AuthTrans19">
        </div>
      </th>

    </tr>


    </div>
    </table>

<!--</form>-->
    <table>
<div class="container">

<!--<form method = "POST">-->
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part II ALE Member Information</h2>
      
      <tr>
        <th>
        <div class="form-group">
        <label for="usr">20 Total number of Forms 1095-C filed by and/or on behalf of ALE Member . . . . . . . . . . . ▶</label>
          <input type="text" class="form-control" id="ALENumberForms20" name="ALENumberForms20">
        </div>
        </th>

        <th>
        <div class="form-group">
        <label for="usr">21. Is ALE Member a member of an Aggregated ALE Group? . . . . . . . . .  . . . . . . . .</label>
        <p>If “No,” do not complete Part IV.</p>
          <input type="Checkbox" id="ALEGroup21" name="ALEGroup21">Yes</input>
          <input type="Checkbox" id="ALEGroup21" name="ALEGroup21">No</input>
        </div>
        </th>
      </tr>

      <tr>
        <th colspan="2">
        <div class="form-group">
        <label for="usr">22 Certifications of Eligibility (select all that apply):</label>
          <input type="Checkbox" id="QOfferMethod22A" name="QOfferMethod22A">A. Qualifying Offer Method</input>
          <input type="Checkbox" id="QOfferTranRelief22B" name="QOfferTranRelief22B">B. Reserved</input>
          <input type="Checkbox" id="Section498022C" name="Section498022C">C. Reserved</input>
          <input type="Checkbox" id="Offer98Percent22D" name="Offer98Percent22D">D. 98% Offer Method</input>
        </div>
        </th>
      </tr>

      <tr>
        <th colspan="2">
        <div class="form-group">
        <label for="usr"></label>
        Signature
          <input type="text"> </input>
          Title
          <input type="text" id="RTitle" name="RTitle"></input>
          Date
          <input type="text"></input>

        </div>
        </th>
      </tr>

      </table>

<!--</form>-->

<table>
<div class="container">

<!--<form method = "POST">-->
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part III ALE Member Information—Monthly</h2>


    <div class="table-wrapper" tabindex="0">


    <tr>
        <th colspan="3">(a) Minimum Essential Coverage
Offer Indicator</th>
        <th>
        <div class="form-group">
            <label for="usr">(b) Section 4980H Full-Time
Employee Count for ALE Member</label>
        </div>
        </th>
        <th><div class="form-group">
            <label for="usr">(c) Total Employee Count
for ALE Member</label>
        </div>
        
        </th>

        <th><div class="form-group">
            <label for="usr">(d) Aggregated
Group Indicator</label>
        </div>
        
        </th>

        <th><div class="form-group">
            <label for="usr">(e) Reserved</label>
        </div>
        
        </th>

      </tr>

      

      <tr>

        <th>23. All 12 Months</th>
        <th>Yes
        <input type="checkbox" class="form-control" id="AllEssential" name="AllEssential">
        </th>
        <th>No
        <input type="checkbox" class="form-control" id="AllEssential" name="AllEssential"></th>
        <th><input type="text" class="form-control" id="AllFTCount" name="AllFTCount"></th>
        <th><input type="text" class="form-control" id="AllTotalCount" name="AllTotalCount"></th>
        <th><input type="checkbox" class="form-control" id="All4980Indicator" name="All4980Indicator"></th>
        <th></th>
        



      </tr>

      <tr>
    <th>24. Jan


<th><input type="checkbox" class="form-control" id="JanEssential" name="JanEssential"></th>
<th><input type="checkbox" class="form-control" id="JanEssential" name="JanEssential"></th>
<th><input type="text" class="form-control" id="JanFTCount" name="JanFTCount"></th>
<th><input type="text" class="form-control" id="JanTotalCount" name="JanTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Jan4980Indicator" name="Jan4980Indicator"></th>
<th></th>




</th>
</tr>


<tr>
    <th>25. Feb


<th><input type="checkbox" class="form-control" id="FebEssential" name="FebEssential"></th>
<th><input type="checkbox" class="form-control" id="FebEssential" name="FebEssential"></th>
<th><input type="text" class="form-control" id="FebFTCount" name="FebFTCount"></th>
<th><input type="text" class="form-control" id="FebTotalCount" name="FebTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Feb4980Indicator" name="Feb4980Indicator"></th>
<th></th>


</th>
</tr>

<tr>
    <th>26. Mar


<th><input type="checkbox" class="form-control" id="MarEssential" name="MarEssential"></th>
<th><input type="checkbox" class="form-control" id="MarEssential" name="MarEssential"></th>
<th><input type="text" class="form-control" id="MarFTCount" name="MarFTCount"></th>
<th><input type="text" class="form-control" id="MarTotalCount" name="MarTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Mar4980Indicator" name="Mar4980Indicator"></th>
<th></th>


</th>
</tr>

<tr>
    <th>27. Apr


<th><input type="checkbox" class="form-control" id="AprEssential" name="AprEssential"></th>
<th><input type="checkbox" class="form-control" id="AprEssential" name="AprEssential"></th>
<th><input type="text" class="form-control" id="AprFTCount" name="AprFTCount"></th>
<th><input type="text" class="form-control" id="AprTotalCount" name="AprTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Apr4980Indicator" name="Apr4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>28. May


    <th><input type="checkbox" class="form-control" id="MayEssential" name="MayEssential"></th>
<th><input type="checkbox" class="form-control" id="MayEssential" name="MayEssential"></th>
<th><input type="text" class="form-control" id="MayFTCount" name="MayFTCount"></th>
<th><input type="text" class="form-control" id="MayTotalCount" name="MayTotalCount"></th>
<th><input type="checkbox" class="form-control" id="May4980Indicator" name="May4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>29. June


    <th><input type="checkbox" class="form-control" id="JunEssential" name="JunEssential"></th>
<th><input type="checkbox" class="form-control" id="JunEssential" name="JunEssential"></th>
<th><input type="text" class="form-control" id="JunFTCount" name="JunFTCount"></th>
<th><input type="text" class="form-control" id="JunTotalCount" name="JunTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Jun4980Indicator" name="Jun4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>30. July


    <th><input type="checkbox" class="form-control" id="JulEssential" name="JulEssential"></th>
<th><input type="checkbox" class="form-control" id="JulEssential" name="JulEssential"></th>
<th><input type="text" class="form-control" id="JulFTCount" name="JulFTCount"></th>
<th><input type="text" class="form-control" id="JulTotalCount" name="JulTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Jul4980Indicator" name="Jul4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>31. Aug


    <th><input type="checkbox" class="form-control" id="AugEssential" name="AugEssential"></th>
<th><input type="checkbox" class="form-control" id="AugEssential" name="AugEssential"></th>
<th><input type="text" class="form-control" id="AugFTCount" name="AugFTCount"></th>
<th><input type="text" class="form-control" id="AugTotalCount" name="AugTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Aug4980Indicator" name="Aug4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>32. Sept


    <th><input type="checkbox" class="form-control" id="SepEssential" name="SepEssential"></th>
<th><input type="checkbox" class="form-control" id="SepEssential" name="SepEssential"></th>
<th><input type="text" class="form-control" id="SepFTCount" name="SepFTCount"></th>
<th><input type="text" class="form-control" id="SepTotalCount" name="SepTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Sep4980Indicator" name="Sep4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>33. Oct


    <th><input type="checkbox" class="form-control" id="OctEssential" name="OctEssential"></th>
<th><input type="checkbox" class="form-control" id="OctEssential" name="OctEssential"></th>
<th><input type="text" class="form-control" id="OctFTCount" name="OctFTCount"></th>
<th><input type="text" class="form-control" id="OctTotalCount" name="OctTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Oct4980Indicator" name="Oct4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>34. Nov


    <th><input type="checkbox" class="form-control" id="NovEssential" name="NovEssential"></th>
<th><input type="checkbox" class="form-control" id="NovEssential" name="NovEssential"></th>
<th><input type="text" class="form-control" id="NovFTCount" name="NovFTCount"></th>
<th><input type="text" class="form-control" id="NovTotalCount" name="NovTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Nov4980Indicator" name="Nov4980Indicator"></th>
<th></th>



</th>
</tr>

<tr>
    <th>35. Dec


    <th><input type="checkbox" class="form-control" id="DecEssential" name="DecEssential"></th>
<th><input type="checkbox" class="form-control" id="DecEssential" name="DecEssential"></th>
<th><input type="text" class="form-control" id="DecFTCount" name="DecFTCount"></th>
<th><input type="text" class="form-control" id="DecTotalCount" name="DecTotalCount"></th>
<th><input type="checkbox" class="form-control" id="Dec4980Indicator" name="Dec4980Indicator"></th>
<th></th>



</th>
</tr>

    </div>


    
<!--</form>-->
</table>





      </table>
      <div class="btn-group btn-group-lg">
        <button type="submit" class="btn btn-primary" name="insert">Insert</button>
      </div>
</form>


<table>
<div class="container">

<form method = "POST">
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part IV Other ALE Members of Aggregated ALE Group</h2>
    <th colspan="9">Enter the names and EINs of Other ALE Members of the Aggregated ALE Group (who were members at any time during the calendar year).</th>


    <div class="table-wrapper" tabindex="0">


    <tr>
        <th>Name</th>
        <th>
        <div class="form-group">
            <label for="usr">EIN</label>
        </div>
        </th>
        <th><div class="form-group">
            <label for="usr">Name</label>
        </div>
        
        </th>

        <th><div class="form-group">
            <label for="usr">EIN</label>
        </div>
        
        </th>

      </tr>

      

      <tr>


        <th>36<input type="text" class="form-control" id="OtherName" name="OtherName"></th>
        <th><input type="text" class="form-control" id="OtherFEIN" name="OtherFEIN"></th>
        <th>51<input type="text" class="form-control" id="" name=""></th>
        <th><input type="text" class="form-control" id="" name=""></th>
     
        



      </tr>

     <!-- 

<tr>
 

<th>37<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>52<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>38<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>53<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>39<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>54<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>40<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>55<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>41<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>56<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>42<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>57<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>43<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>58<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>44<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>59<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>45<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>60<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>46<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>61<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>47<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>62<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>48<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>63<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>49<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>64<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>

<tr>
 

<th>50<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>
<th>65<input type="text" class="form-control" id="" name=""></th>
<th><input type="text" class="form-control" id="" name=""></th>

</tr>
     --> 

    </div> 
    </div>

    <div class="btn-group btn-group-lg">
      <button type="submit" class="btn btn-primary" name="submit">Submit</button>
    </div>
    </table>
</form>

    </div>
</div>

</body>
</html>