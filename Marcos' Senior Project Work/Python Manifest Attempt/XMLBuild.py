from config import *
from xml import *
from appsettings import *
import xml.etree.ElementTree as ET


class XMLBuild(object):

    @staticmethod
    def addExtAttribute(xmlI):
        annualNS = ConfigurationManager.AppSettings["ACAXMLNSHeaderYear"]
        xmlI.WriteAttributeString("xmlns", annualNS)

    @staticmethod
    def addIRSCommonAttribute(xmlI):
        xmlI.WriteAttributeString("xmlns:irs", "urn:us:gov:treasury:irs:common")

    @staticmethod
    def addSchemaAttribute(xmlI):
        xmlI.WriteAttributeString("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")


    @staticmethod
    def addOrgSecurity(xmlI):
        xmlI.WriteAttributeString("xmlns:wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd")

    @staticmethod
    def addACAUIBusiness(xmlI):
        xmlI.WriteAttributeString("xmlns:p", "urn:us:gov:treasury:irs:msg:acauibusinessheader")

    @staticmethod
    def addACABusiness(xmlI):
        xmlI.WriteAttributeString("xmlns:acaBusHeader", "urn:us:gov:treasury:irs:msg:acabusinessheader")

    @staticmethod
    def addACALocation(xmlI):
        xmlI.WriteAttributeString("xsi:schemaLocation", "urn:us:gov:treasury:irs:msg:acauibusinessheader IRS-ACAUserInterfaceHeaderMessage.xsd")

    @staticmethod
    def numvalFalseTrue(evalValue):
        if (((evalValue == "True") or (evalValue == "Yes")) or (evalValue == "1")):
            return "1"
        else:
            return "0"


    @staticmethod
    def num123Eval(evalValue):
        if (((evalValue == "True") or (evalValue == "Yes")) or (evalValue == "1")):
            return "1"
        else:
            return "2"
    
    @staticmethod
    def essentialXMLVal(evalValue):
        if ((evalValue.ToUpper() == "Y")):
            return "1"
        elif ((evalValue.ToUpper() == "N")):
            return "2"
        elif ((evalValue.ToUpper() == "X")):
            return "0"
        else:
            return "Invalid"

    @staticmethod
    def CheckTestingId(inTestId, xmlI):
        if (inTestId != ""):
            testChar = inTestId.Substring(0,1).ToCharArray()[0]

            if (IsDigit(testChar)):
                xmlI.WriteStartElement("TestScenarioId")
                xmlI.WriteString(inTestId)
                xmlI.WriteEndElement()