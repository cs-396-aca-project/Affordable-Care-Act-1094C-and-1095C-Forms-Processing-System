#I tried my best to get the manifest to work but my computer has some issues with running XML and I tried so many libraries/modules
# and the running it never worked so I had to come up with another idea that I show in the demo I uploaded on youtube, code is in github
#and Prof Vander Linden has the zip code.

from sys import *
from collections import *
from generic import *
from secure import *
from io import *
from config import *
from xml import *
import xml.dom.minidom

#from XMLBuild import *


#import CLR
#import CLR.System
#from CLR.System import Console as CLR_Console
#import CLR.System.Xml as CLR_Xml

class XMLManifest(object):
    def __init__(self, p_shippingFile = None, p_manifestLocation = None, 
        p_transmissionType = None, 
        p_taxPayerRecordCount = None, p_employerCount = None, 
        p_formFileSize = None, p_md5 = None):


        self.p_shippingFile = p_shippingFile
        self.p_manifestLocation = p_manifestLocation
        self.p_transmissionType = p_transmissionType
        self.p_taxPayerRecordCount = p_taxPayerRecordCount
        self.p_employerCount = p_employerCount
        self.p_formFileSize = p_formFileSize
        self.p_md5 = p_md5
        self.p_instanceXMLStream = None

    @property
    def p_shippingFile(self):
        return self.p_shippingFile

    @p_shippingFile.setter
    def p_shippingFile(self, value):
        self.p_shippingFile = value

    @property
    def p_manifestLocation(self):
        return self.p_manifestLocation

    @p_manifestLocation.setter
    def p_manifestLocation(self, value):
        self.p_manifestLocation = value

    @property
    def p_transmissionType(self):
        return self.p_transmissionType

    @p_transmissionType.setter
    def p_transmissionType(self, value):
        self.p_transmissionType = value

    @property
    def p_taxPayerRecordCount(self):
        return self.p_taxPayerRecordCount

    @p_taxPayerRecordCount.setter
    def p_taxPayerRecordCount(self, value):
        self.p_taxPayerRecordCount = value

    @property
    def p_employerCount(self):
        return self.p_employerCount
    @p_employerCount.setter
    def p_employerCount(self, value):
        self.p_employerCount = value


    @property
    def p_formFileSize(self):
        return self.p_formFileSize

    @p_formFileSize.setter
    def p_formFileSize(self, value):
        self.p_formFileSize = value
    @property
    def p_md5(self):
        return self.p_md5

    @p_md5.setter
    def p_md5(self, value):
        self.p_md5 = value

def __init__(self, xmlFormFile, manifestName, typeOfSubmission):
        self.p_shippingFile = None
        self.p_manifestLocation = None
        self.p_transmissionType = None
        self.p_taxPayerRecordCount = None
        self.p_employerCount = None
        self.p_formFileSize = None
        self.p_md5 = None
        p_instanceXMLStream = XmlTextWriter(manifestName, None)
        p_shippingFile = xmlFormFile
        p_manifestLocation = manifestName
        p_transmissionType = typeOfSubmission
        getCountFromFormFile(xmlFormFile)

def getCountFromFormFile(self, xmlFormFile):
        try:
            p_taxPayerRecordCount = 0
            p_employerCount = 0
            tempInfo = FileInfo(xmlFormFile)
            p_formFileSize = tempInfo.Length
            stream = File.OpenRead(xmlFormFile)
            reader = XmlTextReader(stream)
                if (reader.NodeType == XmlNodeType.Element):
                    if (reader.Name == "Form1095CUpstreamDetail"):
                        p_taxPayerRecordCount += 1
                    if (reader.Name == "Form1094CUpstreamDetail"):
                        p_employerCount += 1
        except Exception as x:
            raise x


#def beginManifest():


def beginManifest():
    p_instanceXMLStream.WriteStartElement("p:ACAUIBusinessHeader")
    XMLBuild.addSchemaAttribute(p_instanceXMLStream)
    XMLBuild.addExtAttribute(p_instanceXMLStream)
    XMLBuild.addACABusiness(p_instanceXMLStream)
    XMLBuild.addOrgSecurity(p_instanceXMLStream)
    XMLBuild.addIRSCommonAttribute(p_instanceXMLStream)
    XMLBuild.addACAUIBusiness(p_instanceXMLStream)
    XMLBuild.addACALocation(p_instanceXMLStream)

def addBusinessHeader():
    p_instanceXMLStream.WriteStartElement("acaBusHeader:ACABusinessHeader")
    p_instanceXMLStream.WriteStartElement("UniqueTransmissionId")
    p_instanceXMLStream.WriteValue(createTransmissionId())
    p_instanceXMLStream.WriteEndElement()
    p_instanceXMLStream.WriteStartElement("irs:Timestamp")
    p_instanceXMLStream.WriteValue(createTimestamp())
    p_instanceXMLStream.WriteEndElement()
    p_instanceXMLStream.WriteEndElement()

def manifestBody():
    p_instanceXMLStream.WriteStartElement("ACATransmitterManifestReqDtl")
    simpleValueElement("PaymentYr", ConfigurationManager.AppSettings["REPORTTAXYEAR"])
    simpleValueElement("PriorYearDataInd", "0")
    simpleValueElement("irs:EIN", ConfigurationManager.AppSettings["OVDEIN"])
    simpleValueElement("TransmissionTypeCd", p_transmissionType)
    simpleValueElement("TestFileCd", ConfigurationManager.AppSettings["TRANSMISSIONMODE"])
    transmitterGroup()
    CompanyInformation()
    VendorInformation()
    simpleValueElement("TotalPayeeRecordCnt", p_taxPayerRecordCount.ToString())
    simpleValueElement("TotalPayerRecordCnt", p_employerCount.ToString())
    simpleValueElement("SoftwareId", ConfigurationManager.AppSettings["AIRSOFTWAREID"])
    simpleValueElement("FormTypeCd", "1094/1095C")
    simpleValueElement("irs:BinaryFormatCd", "application/xml")
    simpleValueElement("irs:ChecksumAugmentationNum", checkSum256(p_shippingFile))
    simpleValueElement("AttachmentByteSizeNum", p_formFileSize.ToString())
    simpleValueElement("DocumentSystemFileNm", Path.GetFileName(p_shippingFile))
    p_instanceXMLStream.WriteEndElement()

def endManifest():
    p_instanceXMLStream.WriteEndElement()

def VendorInformation():
    p_instanceXMLStream.WriteStartElement("VendorInformationGrp")
    simpleValueElement("VendorCd", "I")
    ContactName()
    simpleValueElement("ContactPhoneNum", ConfigurationManager.AppSettings["CONTACTPHONENUMBER"])
    p_instanceXMLStream.WriteEndElement()


def transmitterGroup():
    p_instanceXMLStream.WriteStartElement("TransmitterNameGrp")
    simpleValueElement("BusinessNameLine1Txt", ConfigurationManager.AppSettings["OVDBUSINESSSNAME"])
    p_instanceXMLStream.WriteEndElement()


def CompanyInformation():
    p_instanceXMLStream.WriteStartElement("CompanyInformationGrp")
    simpleValueElement("CompanyNm", ConfigurationManager.AppSettings["OVDBUSINESSSNAME"])
    MailingAddress()
    ContactName()
    simpleValueElement("ContactPhoneNum", ConfigurationManager.AppSettings["CONTACTPHONENUMBER"])
    p_instanceXMLStream.WriteEndElement()


def ContactName():
    p_instanceXMLStream.WriteStartElement("ContactNameGrp")
    simpleValueElement("PersonFirstNm", ConfigurationManager.AppSettings["CONTACTFIRSTNAME"])
    simpleValueElement("PersonLastNm", ConfigurationManager.AppSettings["CONTACTLASTNAME"])
    p_instanceXMLStream.WriteEndElement()


def MailingAddress():
    p_instanceXMLStream.WriteStartElement("MailingAddressGrp")
    p_instanceXMLStream.WriteStartElement("USAddressGrp")
    simpleValueElement("AddressLine1Txt", ConfigurationManager.AppSettings["ADDRESSLINE1"])
    simpleValueElement("irs:CityNm", ConfigurationManager.AppSettings["ADDRESSCITYNAME"])
    simpleValueElement("USStateCd", ConfigurationManager.AppSettings["ADDRESSSTATECODE"])
    simpleValueElement("irs:USZIPCd", ConfigurationManager.AppSettings["ADDRESSZIPCODE"])
    p_instanceXMLStream.WriteEndElement()
    p_instanceXMLStream.WriteEndElement()


def simpleValueElement(elementName, elementValue):
    p_instanceXMLStream.WriteStartElement(elementName)
    p_instanceXMLStream.WriteValue(elementValue)
    p_instanceXMLStream.WriteEndElement()


def addBusinessHeader():
    p_instanceXMLStream.WriteStartElement("acaBusHeader:ACABusinessHeader")
    p_instanceXMLStream.WriteStartElement("UniqueTransmissionId")
    p_instanceXMLStream.WriteValue(createTransmissionId())
    p_instanceXMLStream.WriteEndElement()
    p_instanceXMLStream.WriteStartElement("irs:Timestamp")
    p_instanceXMLStream.WriteValue(createTimestamp())
    p_instanceXMLStream.WriteEndElement()
    p_instanceXMLStream.WriteEndElement()

def checkMD5(filename):
    with MD5.Create() as md5:
        with File.OpenRead(filename) as stream:
            rValue = BitConverter.ToString(md5.ComputeHash(stream)).Replace("-", String.Empty)
    return rValue


def checkSum256(filename):
    with File.OpenRead(filename) as stream:
        sha = SHA256Managed()
        checksum = sha.ComputeHash(stream)
        return BitConverter.ToString(checksum).Replace("-", String.Empty)


def createTimestamp():
    rValue = ""
    rValue = DateTime.Now.ToUniversalTime().ToString("yyyy\'-\'MM\'-\'dd\'T\'HH\':\'mm\':\'ss\'Z\'")
    return rValue


def createTransmissionId():
    mUUID = System.Guid.NewGuid().ToString()
    appId = ConfigurationManager.AppSettings["IRSAPPLICATIONID"]
    transId = ConfigurationManager.AppSettings["TCCID"]
    reservedField = ""
    transMode = ConfigurationManager.AppSettings["TRANSACTIONTYPE"]
    rValue = ((((((((mUUID + ":") + appId) + ":") + transId) + ":") + reservedField) + ":") + transMode)
    return rValue
