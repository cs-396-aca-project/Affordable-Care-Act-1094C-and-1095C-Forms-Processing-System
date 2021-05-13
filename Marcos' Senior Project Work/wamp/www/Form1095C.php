<?php 

/*
Name : Marcos Hernandez
Senior Project
*/

include("connection.php");


//Same process works here like in the other form
function getData(){
    $data = array();
    $data[1] = $_POST['REName'];
    $data[2] = $_POST['RSAddress'];
    $data[3] = $_POST['RCity'];
    $data[5] = $_POST['RContactPhone'];
    $data[6] = $_POST['RState'];
    $data[7] = $_POST['RGZipCode'];
    $data[4] = $_POST['EmployerFilingBaseId'];
    return $data;
}


if(isset($_POST['insert'])){
  $info = getData();
  $insert = "INSERT INTO [YEEmployerBaseFiled] ([REName], 
  [RSAddress], [RCity], [EmployerFilingBaseId], [RContactPhone], [RState], [RGZipCode]) 
  VALUES ('$info[1]', '$info[2]', '$info[3]', '$info[4]', '$info[5]', '$info[6]', '$info[7]')";

  $result = odbc_exec($connect, $insert);
}


function getMoreData(){
    $data[1] = $_POST['L1_FirstName'];
    $data[2] = $_POST['L1_LastName'];
    $data[3] = $_POST['IndividualSSN'];
    $data[4] = $_POST['L3_StreetAddress'];
    $data[5] = $_POST['L4_City'];
    $data[6] = $_POST['L5_State'];
    $data[7] = $_POST['L6_ZipCode'];
    $data[8] = $_POST['L1_EmployeeName'];
    $data[9] = $_POST['EmpID'];
    $data[10] = $_POST['L14_Jan'];
    $data[11] = $_POST['L14_Feb'];
    $data[12] = $_POST['L14_Mar'];
    $data[13] = $_POST['L14_Apr'];
    $data[14] = $_POST['L14_May'];
    $data[15] = $_POST['L14_Jun'];
    $data[16] = $_POST['L14_Jul'];
    $data[17] = $_POST['L14_Aug'];
    $data[18] = $_POST['L14_Sep'];
    $data[19] = $_POST['L14_Oct'];
    $data[20] = $_POST['L14_Nov'];
    $data[21] = $_POST['L14_Dec'];
    $data[22] = $_POST['L15_Jan'];
    $data[23] = $_POST['L15_Feb'];
    $data[24] = $_POST['L15_Mar'];
    $data[25] = $_POST['L15_Apr'];
    $data[26] = $_POST['L15_May'];
    $data[27] = $_POST['L15_Jun'];
    $data[28] = $_POST['L15_Jul'];
    $data[29] = $_POST['L15_Aug'];
    $data[30] = $_POST['L15_Sep'];
    $data[31] = $_POST['L15_Oct'];
    $data[32] = $_POST['L15_Nov'];
    $data[33] = $_POST['L15_Dec'];
    $data[34] = $_POST['L16_Jan'];
    $data[35] = $_POST['L16_Feb'];
    $data[36] = $_POST['L16_Mar'];
    $data[37] = $_POST['L16_Apr'];
    $data[38] = $_POST['L16_May'];
    $data[39] = $_POST['L16_Jun'];
    $data[40] = $_POST['L16_Jul'];
    $data[41] = $_POST['L16_Aug'];
    $data[42] = $_POST['L16_Sep'];
    $data[43] = $_POST['L16_Oct'];
    $data[44] = $_POST['L16_Nov'];
    $data[45] = $_POST['L16_Dec'];
    $data[46] = $_POST['L17_Jan'];
    $data[47] = $_POST['L17_Feb'];
    $data[48] = $_POST['L17_Mar'];
    $data[49] = $_POST['L17_Apr'];
    $data[50] = $_POST['L17_May'];
    $data[51] = $_POST['L17_Jun'];
    $data[52] = $_POST['L17_Jul'];
    $data[53] = $_POST['L17_Aug'];
    $data[54] = $_POST['L17_Sep'];
    $data[55] = $_POST['L17_Oct'];
    $data[56] = $_POST['L17_Nov'];
    $data[57] = $_POST['L17_Dec'];
    $data[58] = $_POST['L14_All'];
    $data[59] = $_POST['L15_All'];
    $data[60] = $_POST['L16_All'];
    $data[61] = $_POST['L17_All'];

      return $data;

}

if(isset($_POST['enter'])){
  $info2 = getMoreData();
  $insert2 = "INSERT INTO [YE1095Head] ([L1_FirstName], 
  [L1_LastName], [IndividualSSN], [L3_StreetAddress], [L4_City], [L5_State], [L6_ZipCode], [L1_EmployeeName], [EmployerFilingBaseId]
  
  
  , [L14_Jan], [L14_Feb], [L14_Mar], [L14_Apr], [L14_May], [L14_Jun], [L14_Jul], [L14_Aug], [L14_Sep], [L14_Oct], [L14_Nov], [L14_Dec]
  , [L15_Jan], [L15_Feb], [L15_Mar], [L15_Apr], [L15_May], [L15_Jun], [L15_Jul], [L15_Aug], [L15_Sep], [L15_Oct], [L15_Nov], [L15_Dec]
  , [L16_Jan], [L16_Feb], [L16_Mar], [L16_Apr], [L16_May], [L16_Jun], [L16_Jul], [L16_Aug], [L16_Sep], [L16_Oct], [L16_Nov], [L16_Dec]
  , [L17_Jan], [L17_Feb], [L17_Mar], [L17_Apr], [L17_May], [L17_Jun], [L17_Jul], [L17_Aug], [L17_Sep], [L17_Oct], [L17_Nov], [L17_Dec]
  , [L14_All], [L15_All], [L16_All], [L17_All]) 
  VALUES ('$info2[1]', '$info2[2]', '$info2[3]', '$info2[4]', '$info2[5]', '$info2[6]', '$info2[7]', '$info2[8]', '$info2[9]'
  , '$info2[10]', '$info2[11]', '$info2[12]', '$info2[13]', '$info2[14]', '$info2[15]', '$info2[16]', '$info2[17]', '$info2[18]', '$info2[19]', '$info2[20]', '$info2[21]'
  , '$info2[22]', '$info2[23]', '$info2[24]', '$info2[25]', '$info2[26]', '$info2[27]', '$info2[28]', '$info2[29]', '$info2[30]', '$info2[31]', '$info2[32]', '$info2[33]'
  , '$info2[34]', '$info2[35]', '$info2[36]', '$info2[37]', '$info2[38]', '$info2[39]', '$info2[40]', '$info2[41]', '$info2[42]', '$info2[43]', '$info2[44]', '$info2[45]'
  , '$info2[46]', '$info2[47]', '$info2[48]', '$info2[49]', '$info2[50]', '$info2[51]', '$info2[52]', '$info2[53]', '$info2[54]', '$info2[55]', '$info2[56]', '$info2[57]'
  , '$info2[58]', '$info2[59]', '$info2[60]', '$info2[61]')";
  $result = odbc_exec($connect, $insert2);
  //print_r($info2);
}

$EmployerArray = array();

$query = "SELECT * FROM YEEmployerBaseFiled";
$resultQuery = odbc_exec($connect, $query);
while($item = odbc_fetch_object($resultQuery)){
  $EmployerArray[] = array('ID' => $item->EmployerFilingBaseId, 'Name' => $item->REName);
};

function getMoreMoreData(){
    $data = array();
    $data[1] = $_POST['CoveredFname'];
    $data[2] = $_POST['CoveredMI'];
    $data[3] = $_POST['CoveredLName'];
    $data[4] = $_POST['CoveredSSN'];
    $data[5] = $_POST['CoveredDOB'];
    $data[6] = ($_POST['CoveredAll'] == '' )? 0:1 ;
    $data[7] = ($_POST['Jan'] == '' )? 0:1 ;
    $data[8] = ($_POST['Feb'] == '' )? 0:1 ;
    $data[9] = ($_POST['Mar'] == '' )? 0:1 ;
    $data[10] = ($_POST['Apr'] == '' )? 0:1 ;
    $data[11] = ($_POST['May'] == '' )? 0:1 ;
    $data[12] = ($_POST['Jun'] == '' )? 0:1 ;
    $data[13] = ($_POST['Jul'] == '' )? 0:1 ;
    $data[14] = ($_POST['Aug'] == '' )? 0:1 ;
    $data[15] = ($_POST['Sep'] == '' )? 0:1 ;
    $data[16] = ($_POST['Oct'] == '' )? 0:1 ;
    $data[17] = ($_POST['Nov'] == '' )? 0:1 ;
    $data[18] = ($_POST['Dec'] == '' )? 0:1 ;
    return $data;

}


if(isset($_POST['submit'])){
  $info = getMoreMoreData();
  $insert3 = "INSERT INTO [YE1095Covered] ([CoveredFname], 
  [CoveredMI], [CoveredLName], [CoveredSSN], [CoveredDOB], [CoveredAll], [Jan], [Feb], [Mar], [Apr], [May], [Jun], [Jul], [Aug], [Sep]
  , [Oct], [Nov], [Dec]) 
  VALUES ('$info[1]', '$info[2]', '$info[3]', '$info[4]', '$info[5]', '$info[6]', '$info[7]', '$info[8]', '$info[9]', '$info[10]', '$info[11]', '$info[12]', '$info[13]'
  , '$info[14]', '$info[15]', '$info[16]', '$info[17]', '$info[18]')";

  $result = odbc_exec($connect, $insert3);
  //print_r($info);
}

?>


<!DOCTYPE html>
<html lang="en">
<head>
  <title>1095C Form</title>
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
  <h1>1095C Form</h1>
  <a href="index.php"><button>Return</button></a>
</div>
<h2>Part I: Employee</h2>
<table>
<div class="container">


<form method = "POST">
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Applicable Large Employer Member (Employer)</h2>
      <th>
      
      <label for="usr">1. Name of Employer:</label>
        <input type="text" class="form-control" id="REName" name="REName">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">2. Employer Identification Number (EIN): </label>
        <input type="text" class="form-control" id="EmployerFilingBaseId" name="EmployerFilingBaseId">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">3. Street Address (including room or suite no.):</label>
        <input type="text" class="form-control" id="RSAddress" name="RSAddress">
        </th>

      <div class="form-group">
      <th>
      <label for="usr">4. Contact Phone Number: </label>
        <input type="text" class="form-control" id="RContactPhone" name="RContactPhone">
        </th>
        </tr>
      </div>

    <tr>
    <th>
      <div class="form-group">
      <label for="usr">5. City or Town: </label>
        <input type="text" class="form-control" id="RCity" name="RCity">
      </div>
    </th>

    <th>
        <div class="form-group">
        <label for="usr">6. State or Province: </label>
          <input maxlength="2" type="text" class="form-control" id="RState" name="RState">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">7. Country and ZIP or foreign postal code: </label>
          <input type="text" class="form-control" id="RGZipCode" name="RGZipCode">
        </div>
      </th>

      </tr>

      </table>
      <div class="btn-group btn-group-lg">
        <button type="submit" class="btn btn-primary" name="insert">Insert</button>
      </div>
    
    </div>

</form>


<table>
<div class="container">
<form method = "POST">



  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <th colspan="4">
      <label>Employer Name</label>
      <select class="form-control" name="EmpID">
        <option>
          Select Employer
        </option>
        <?php foreach ($EmployerArray as $value) {?>
            <option value="<?php echo $value['ID']?>"> <?php echo $value['Name']?> </option>
        
        
        <?php }?>

      </select>
      </th>
    </tr>
      <tr>
      <th>
      <label for="usr">8. First Name:</label>
        <input type="text" class="form-control" id="L1_EmployeeName" name="L1_EmployeeName">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">M.I</label>
        <input type="text" class="form-control" id="L1_FirstName" name="L1_FirstName">
        </th>
      </div>

      <div class="form-group">
      <th>
      <label for="usr">Last Name:</label>
        <input type="text" class="form-control" id="L1_LastName" name="L1_LastName">
        </th>

      <div class="form-group">
      <th>
      <label for="usr">9. Social Security Number(SSN): </label>
        <input type="text" class="form-control" id="IndividualSSN" name="IndividualSSN">
        </th>
        </tr>
      </div>

    <tr>
    <th>
      <div class="form-group">
      <label for="usr">10. Street Address(including apartment no.): </label>
        <input type="text" class="form-control" id="L3_StreetAddress" name="L3_StreetAddress">
      </div>
    </th>

    <th>
        <div class="form-group">
        <label for="usr">11. City or town: </label>
          <input type="text" class="form-control" id="L4_City" name="L4_City">
        </div>
      </th>

      <th>
        <div class="form-group" >
        <label for="usr">12. State or Province: </label>
          <input type="text" class="form-control" id="L5_State" name="L5_State">
        </div>
      </th>

      <th>
        <div class="form-group">
        <label for="usr">13. Country and ZIP or foreign postal code: </label>
          <input type="text" class="form-control" id="L6_ZipCode" name="L6_ZipCode">
        </div>
      </th>
      </tr>
      

    <tr>

    </tr>


    </div>
    </table>


<table>
<div class="container">

  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part II</h2>

    </tr>

    <div class="table-wrapper" tabindex="0">


    <tr>
        <th colspan="4">Employee Offer of Coverage</th>
        <th colspan="4">
        <div class="form-group">
            <label for="usr">Employee's Age on January 1</label>
            <input type="text" class="form-control" id="" name="">
        </div>
        </th>
        <th colspan="5"><div class="form-group">
            <label for="usr">Plan Start Month (enter 2-digit number):</label>
            <input type="text" class="form-control" id="" name="">
        </div>
        
        </th>

      </tr>

      

      <tr>
        <th></th>
        <th>All 12 Months</th>
        <th>Jan</th>
        <th>Feb</th>
        <th>Mar</th>
        <th>Apr</th>
        <th>May</th>
        <th>June</th>
        <th>July</th>
        <th>Aug</th>
        <th>Sept</th>
        <th>Oct</th>
        <th>Nov</th>
        <th>Dec</th>


      </tr>


      <tr>
    <th>14 Offer of
Coverage (enter
required code)

<th><input max-length="2" type="text" class="form-control" id="L14_All" name="L14_All"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Jan" name="L14_Jan"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Feb" name="L14_Feb"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Mar" name="L14_Mar"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Apr" name="L14_Apr"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_May" name="L14_May"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Jun" name="L14_Jun"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Jul" name="L14_Jul"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Aug" name="L14_Aug"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Sep" name="L14_Sep"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Oct" name="L14_Oct"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Nov" name="L14_Nov"></th>
<th><input max-length="2" type="text" class="form-control" id="L14_Dec" name="L14_Dec"></th>



</th>
</tr>


<tr>
    <th>15 Employee
Required
Contribution

<th>$ <input type="number" class="form-control" id="L15_All" name="L15_All"></th>
<th>$ <input type="number" class="form-control" id="L15_Jan" name="L15_Jan"></th>
<th>$ <input type="number" class="form-control" id="L15_Feb" name="L15_Feb"></th>
<th>$ <input type="number" class="form-control" id="L15_Mar" name="L15_Mar"></th>
<th>$ <input type="number" class="form-control" id="L15_Apr" name="L15_Apr"></th>
<th>$ <input type="number" class="form-control" id="L15_May" name="L15_May"></th>
<th>$ <input type="number" class="form-control" id="L15_Jun" name="L15_Jun"></th>
<th>$ <input type="number" class="form-control" id="L15_Jul" name="L15_Jul"></th>
<th>$ <input type="number" class="form-control" id="L15_Aug" name="L15_Aug"></th>
<th>$ <input type="number" class="form-control" id="L15_Sep" name="L15_Sep"></th>
<th>$ <input type="number" class="form-control" id="L15_Oct" name="L15_Oct"></th>
<th>$ <input type="number" class="form-control" id="L15_Nov" name="L15_Nov"></th>
<th>$ <input type="number" class="form-control" id="L15_Dec" name="L15_Dec"></th>

</th>
</tr>

<tr>
    <th>16 Section 4980H
Safe Harbor and
Other Relief (enter
code, if applicable)

<th><input type="text" class="form-control" id="L16_All" name="L16_All"></th>
<th><input type="text" class="form-control" id="L16_Jan" name="L16_Jan"></th>
<th><input type="text" class="form-control" id="L16_Feb" name="L16_Feb"></th>
<th><input type="text" class="form-control" id="L16_Mar" name="L16_Mar"></th>
<th><input type="text" class="form-control" id="L16_Apr" name="L16_Apr"></th>
<th><input type="text" class="form-control" id="L16_May" name="L16_May"></th>
<th><input type="text" class="form-control" id="L16_Jun" name="L16_Jun"></th>
<th><input type="text" class="form-control" id="L16_Jul" name="L16_Jul"></th>
<th><input type="text" class="form-control" id="L16_Aug" name="L16_Aug"></th>
<th><input type="text" class="form-control" id="L16_Sep" name="L16_Sep"></th>
<th><input type="text" class="form-control" id="L16_Oct" name="L16_Oct"></th>
<th><input type="text" class="form-control" id="L16_Nov" name="L16_Nov"></th>
<th><input type="text" class="form-control" id="L16_Dec" name="L16_Dec"></th>

</th>
</tr>

<tr>
    <th>17 ZIP Code

<th><input type="text" class="form-control" id="L17_All" name="L17_All"></th>
<th><input type="text" class="form-control" id="L17_Jan" name="L17_Jan"></th>
<th><input type="text" class="form-control" id="L17_Feb" name="L17_Feb"></th>
<th><input type="text" class="form-control" id="L17_Mar" name="L17_Mar"></th>
<th><input type="text" class="form-control" id="L17_Apr" name="L17_Apr"></th>
<th><input type="text" class="form-control" id="L17_May" name="L17_May"></th>
<th><input type="text" class="form-control" id="L17_Jun" name="L17_Jun"></th>
<th><input type="text" class="form-control" id="L17_Jul" name="L17_Jul"></th>
<th><input type="text" class="form-control" id="L17_Aug" name="L17_Aug"></th>
<th><input type="text" class="form-control" id="L17_Sep" name="L17_Sep"></th>
<th><input type="text" class="form-control" id="L17_Oct" name="L17_Oct"></th>
<th><input type="text" class="form-control" id="L17_Nov" name="L17_Nov"></th>
<th><input type="text" class="form-control" id="L17_Dec" name="L17_Dec"></th>


</th>
</tr>
      

<div class="btn-group btn-group-lg">
      <button type="submit" class="btn btn-primary" name="enter">Enter</button>
    </div>

    </div>
    
</form>
</table>


<table>
<div class="container">

<form method = "POST">
  <div class="row">
    <div class="col-sm-4">
    
    <div class="form-group">
    <tr>
    <h2>Part III</h2>

    </tr>

    <div class="table-wrapper" tabindex="0">


    <tr>
        <th colspan="19">Covered Individual
        <p>If Employer provided self-insured coverage, check the box and enter the information for each 
        individual enrolled in coverage, including the employee. <input type="checkbox" class="form-control"></p></th>
      </tr>

      

      <tr>

        <th colspan="4">(a) Name of covered individual(s)
First name, middle initial, last name</th>
        <th>(b) SSN or other TIN </th>
        <th>(c) DOB (if SSN or other
TIN is not available)</th>
        <th>(d) Covered
all 12 month</th>

<th colspan="13">(e) Months of coverage</th>

      </tr>

      <tr>
    <th>18.


<th>F.N<input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th>M.I<input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th>L.N<input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th>Jan<input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th>Feb<input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th>Mar<input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th>Apr<input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th>May<input type="checkbox" class="form-control" id="May" name="May"></th>
<th>June<input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th>July<input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th>Aug<input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th>Sept<input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th>Oct<input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th>Nov<input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th>Dec<input type="checkbox" class="form-control" id="Dec" name="Dec"></th>


</th>
</tr>

<!--
<tr>
    <th>19


<th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>



</th>
</tr>

<tr>
    <th>20


<th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>



</th>
</tr>

<tr>
    <th>21


<th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>22


<th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>23


<th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>24


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>25


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>26


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>27


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>28


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>29


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>

<tr>
    <th>30


    <th><input type="text" class="form-control" id="CoveredFname" name="CoveredFname"></th>
<th><input type="text" class="form-control" id="CoveredMI" name="CoveredMI"></th>
<th><input type="text" class="form-control" id="CoveredLName" name="CoveredLName"></th>
<th><input type="text" class="form-control" id="CoveredSSN" name="CoveredSSN"></th>
<th><input type="text" class="form-control" id="CoveredDOB" name="CoveredDOB"></th>
<th><input type="checkbox" class="form-control" id="CoveredAll" name="CoveredAll"></th>
<th><input type="checkbox" class="form-control" id="Jan" name="Jan"></th>
<th><input type="checkbox" class="form-control" id="Feb" name="Feb"></th>
<th><input type="checkbox" class="form-control" id="Mar" name="Mar"></th>
<th><input type="checkbox" class="form-control" id="Apr" name="Apr"></th>
<th><input type="checkbox" class="form-control" id="May" name="May"></th>
<th><input type="checkbox" class="form-control" id="Jun" name="Jun"></th>
<th><input type="checkbox" class="form-control" id="Jul" name="Jul"></th>
<th><input type="checkbox" class="form-control" id="Aug" name="Aug"></th>
<th><input type="checkbox" class="form-control" id="Sep" name="Sep"></th>
<th><input type="checkbox" class="form-control" id="Oct" name="Oct"></th>
<th><input type="checkbox" class="form-control" id="Nov" name="Nov"></th>
<th><input type="checkbox" class="form-control" id="Dec" name="Dec"></th>




</th>
</tr>
      -->
<div class="btn-group btn-group-lg">
      <button type="submit" class="btn btn-primary" name="submit">Submit</button>
    </div>
    </div>

</form>
</table>

    
    </div>
</div>

</body>
</html>