CREATE DATABASE rawtest2
GO

USE [rawtest2]
GO
/****** Object:  Table [dbo].[YE1094OtherALE]    Script Date: 11/19/2020 8:38:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[YE1094OtherALE](
	[EmployerFilingBaseId] [nvarchar](11) NULL,
	[OtherFEIN] [nvarchar](11) NULL,
	[OtherName] [nvarchar](255) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[YE1095Covered]    Script Date: 11/19/2020 8:38:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[YE1095Covered](
	[CBaseID] [int] IDENTITY(1,1) NOT NULL,
	[EmployerFilingBaseId] [nvarchar](11) NULL,
	[L8_RptFEIN] [nvarchar](11) NULL,
	[CLine] [smallint] NULL,
	[IndividualSSN] [nvarchar](11) NULL,
	[LatestRMonth] [smallint] NULL,
	[CoveredSSN] [nvarchar](9) NULL,
	[CoveredLName] [nvarchar](50) NULL,
	[CoveredFname] [nvarchar](50) NULL,
	[CoveredMI] [nvarchar](1) NULL,
	[CoveredDOB] [nvarchar](11) NULL,
	[CoveredAll] [bit] NULL,
	[Jan] [bit] NULL,
	[Feb] [bit] NULL,
	[Mar] [bit]  NULL,
	[Apr] [bit] NULL,
	[May] [bit] NULL,
	[Jun] [bit] NULL,
	[Jul] [bit] NULL,
	[Aug] [bit] NULL,
	[Sep] [bit] NULL,
	[Oct] [bit] NULL,
	[Nov] [bit] NULL,
	[Dec] [bit] NULL,
PRIMARY KEY CLUSTERED 
(
	[CBaseID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[YE1095Head]    Script Date: 11/19/2020 8:38:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[YE1095Head](
	[EmployerFilingBaseId] [nvarchar](11) NOT NULL,
	[L8_RptFEIN] [nvarchar](11) NULL,
	[IndividualSSN] [nvarchar](11) NOT NULL,
	[LatestRMonth] [smallint] NULL,
	[L2_IndividualSSN] [nvarchar](11) NULL,
	[L1_EmployeeName] [nvarchar](80) NULL,
	[L1_FirstName] [nvarchar](50) NULL,
	[L1_LastName] [nvarchar](50) NULL,
	[L3_StreetAddress] [nvarchar](255) NULL,
	[L4_City] [nvarchar](40) NULL,
	[L5_State] [nvarchar](2) NULL,
	[L6_ZipCode] [nvarchar](9) NULL,
	[PartIII_EmployerProvidedSelfIns] [bit] NULL,
	[GeneratedDate] [datetime2](7) NULL,
	[Corrected] [bit] NULL,
	[Void] [bit] NULL,
	[Frozen] [bit] NULL,
	[Comment] [nvarchar](255) NULL,
	[L14_All] [nvarchar](2) NULL,
	[L14_Jan] [nvarchar](2) NULL,
	[L14_Feb] [nvarchar](2) NULL,
	[L14_Mar] [nvarchar](2) NULL,
	[L14_Apr] [nvarchar](2) NULL,
	[L14_May] [nvarchar](2) NULL,
	[L14_Jun] [nvarchar](2) NULL,
	[L14_Jul] [nvarchar](2) NULL,
	[L14_Aug] [nvarchar](2) NULL,
	[L14_Sep] [nvarchar](2) NULL,
	[L14_Oct] [nvarchar](2) NULL,
	[L14_Nov] [nvarchar](2) NULL,
	[L14_Dec] [nvarchar](2) NULL,
	[L15_All] [money] NULL,
	[L15_Jan] [money] NULL,
	[L15_Feb] [money] NULL,
	[L15_Mar] [money] NULL,
	[L15_Apr] [money] NULL,
	[L15_May] [money] NULL,
	[L15_Jun] [money] NULL,
	[L15_Jul] [money] NULL,
	[L15_Aug] [money] NULL,
	[L15_Sep] [money] NULL,
	[L15_Oct] [money] NULL,
	[L15_Nov] [money] NULL,
	[L15_Dec] [money] NULL,
	[L16_All] [nvarchar](255) NULL,
	[L16_Jan] [nvarchar](2) NULL,
	[L16_Feb] [nvarchar](2) NULL,
	[L16_Mar] [nvarchar](2) NULL,
	[L16_Apr] [nvarchar](2) NULL,
	[L16_May] [nvarchar](2) NULL,
	[L16_Jun] [nvarchar](2) NULL,
	[L16_Jul] [nvarchar](2) NULL,
	[L16_Aug] [nvarchar](2) NULL,
	[L16_Sep] [nvarchar](2) NULL,
	[L16_Oct] [nvarchar](2) NULL,
	[L16_Nov] [nvarchar](2) NULL,
	[L16_Dec] [nvarchar](2) NULL,
	[L17_All] [nvarchar](5) NULL,
	[L17_Jan] [nvarchar](5) NULL,
	[L17_Feb] [nvarchar](5) NULL,
	[L17_Mar] [nvarchar](5) NULL,
	[L17_Apr] [nvarchar](5) NULL,
	[L17_May] [nvarchar](5) NULL,
	[L17_Jun] [nvarchar](5) NULL,
	[L17_Jul] [nvarchar](5) NULL,
	[L17_Aug] [nvarchar](5) NULL,
	[L17_Sep] [nvarchar](5) NULL,
	[L17_Oct] [nvarchar](5) NULL,
	[L17_Nov] [nvarchar](5) NULL,
	[L17_Dec] [nvarchar](5) NULL,
	[SendRecord] [bit] NULL,
	[PStartMonth] [nvarchar](2) NULL,
	[DTestId] [nvarchar](4) NULL,
	[IRSTransmissionID] [nvarchar](20) NULL,
	[SubmissionId] [smallint] NULL,
	[TimeStamp] [timestamp] NULL,
 CONSTRAINT [PK_Head1095] PRIMARY KEY NONCLUSTERED 
(
	[EmployerFilingBaseId] ASC,
	[IndividualSSN] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[YEBFLog]    Script Date: 11/19/2020 8:38:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[YEBFLog](
	[ActionType] [nchar](1) NULL,
	[UserId] [nvarchar](30)NULL,
	[LogDate] [datetime] NULL,
	[EmployerFilingBaseId] [nvarchar](11) NULL,
	[Corrected] [bit] NULL,
	[Frozen] [bit] NULL,
	[RYearFiled] [nvarchar](4) NULL,
	[REName] [nvarchar](80) NULL,
	[RSAddress] [nvarchar](255) NULL,
	[RCity] [nvarchar](40) NULL,
	[RState] [nvarchar](2) NULL,
	[RZipCode] [nvarchar](255) NULL,
	[RContactName] [nvarchar](80) NULL,
	[RContactPhone] [nvarchar](255) NULL,
	[RGFEIN] [nvarchar](9) NULL,
	[RGName] [nvarchar](80) NULL,
	[RGAddress] [nvarchar](80) NULL,
	[RGCity] [nvarchar](40) NULL,
	[RGState] [nvarchar](2) NULL,
	[RGContactPhone] [nvarchar](10) NULL,
	[RGContact] [nvarchar](80) NULL,
	[RGZipCode] [nvarchar](255) NULL,
	[RGContactName] [nvarchar](80) NULL,
	[NumberForms18] [int] NULL,
	[AuthTrans19] [bit] NULL,
	[ALENumberForms20] [int] NULL,
	[ALEGroup21] [bit] NULL,
	[QOfferMethod22A] [bit] NULL,
	[QOfferTranRelief22B] [bit] NULL,
	[Section498022C] [bit] NULL,
	[Offer98Percent22D] [bit] NULL,
	[RTitle] [nvarchar](80) NULL,
	[AllEssential] [nvarchar](1) NULL,
	[AllFTCount] [int] NULL,
	[AllTotalCount] [int] NULL,
	[AllAggregateGroup] [bit] NULL,
	[All4980Indicator] [nvarchar](10) NULL,
	[JanEssential] [nvarchar](1) NULL,
	[JanFTCount] [int] NULL,
	[JanTotalCount] [int] NULL,
	[JanAggregateGroup] [bit] NULL,
	[Jan4980Indicator] [nvarchar](10) NULL,
	[FebEssential] [nvarchar](1) NULL,
	[FebFTCount] [int] NULL,
	[FebTotalCount] [int] NULL,
	[FebAggregateGroup] [bit] NULL,
	[Feb4980Indicator] [nvarchar](10) NULL,
	[MarEssential] [nvarchar](1) NULL,
	[MarFTCount] [int] NULL,
	[MarTotalCount] [int] NULL,
	[MarAggregateGroup] [bit] NULL,
	[Mar4980Indicator] [nvarchar](10) NULL,
	[AprEssential] [nvarchar](1) NULL,
	[AprFTCount] [int] NULL,
	[AprTotalCount] [int] NULL,
	[AprAggregateGroup] [bit] NULL,
	[Apr4980Indicator] [nvarchar](10) NULL,
	[MayEssential] [nvarchar](1) NULL,
	[MayFTCount] [int] NULL,
	[MayTotalCount] [int] NULL,
	[MayAggregateGroup] [bit] NULL,
	[May4980Indicator] [nvarchar](10) NULL,
	[JunEssential] [nvarchar](1) NULL,
	[JunFTCount] [int] NULL,
	[JunTotalCount] [int] NULL,
	[JunAggregateGroup] [bit] NULL,
	[Jun4980Indicator] [nvarchar](10) NULL,
	[JulEssential] [nvarchar](1) NULL,
	[JulFTCount] [int] NULL,
	[JulTotalCount] [int] NULL,
	[JulAggregateGroup] [bit] NULL,
	[Jul4980Indicator] [nvarchar](10) NULL,
	[AugEssential] [nvarchar](1) NULL,
	[AugFTCount] [int] NULL,
	[AugTotalCount] [int] NULL,
	[AugAggregateGroup] [bit] NULL,
	[Aug4980Indicator] [nvarchar](10) NULL,
	[SepEssential] [nvarchar](1)NULL,
	[SepFTCount] [int] NULL,
	[SepTotalCount] [int] NULL,
	[SepAggregateGroup] [bit] NULL,
	[Sep4980Indicator] [nvarchar](10) NULL,
	[OctEssential] [nvarchar](1)NULL,
	[OctFTCount] [int] NULL,
	[OctTotalCount] [int] NULL,
	[OctAggregateGroup] [bit] NULL,
	[Oct4980Indicator] [nvarchar](10) NULL,
	[NovEssential] [nvarchar](1) NULL,
	[NovFTCount] [int] NULL,
	[NovTotalCount] [int] NULL,
	[NovAggregateGroup] [bit] NULL,
	[Nov4980Indicator] [nvarchar](10) NULL,
	[DecEssential] [nvarchar](1) NULL,
	[DecFTCount] [int] NULL,
	[DecTotalCount] [int] NULL,
	[DecAggregateGroup] [bit] NULL,
	[Dec4980Indicator] [nvarchar](10) NULL,
	[Total1095Count] [smallint] NULL,
	[HTestId] [nvarchar](4) NULL,
	[IRSTransmissionID] [nvarchar](20) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[YEEmployerBaseFiled]    Script Date: 11/19/2020 8:38:00 AM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[YEEmployerBaseFiled](
	[EmployerFilingBaseId] [nvarchar](11) not NULL,
	[Corrected] [bit] NULL,
	[Frozen] [bit] NULL,
	[RYearFiled] [nvarchar](4) NULL,
	[REName] [nvarchar](80) NULL,
	[RSAddress] [nvarchar](255) NULL,
	[RCity] [nvarchar](40) NULL,
	[RState] [nvarchar](2) NULL,
	[RZipCode] [nvarchar](255) NULL,
	[RContactName] [nvarchar](80) NULL,
	[RContactPhone] [nvarchar](255) NULL,
	[RGFEIN] [nvarchar](9) NULL,
	[RGName] [nvarchar](80) NULL,
	[RGAddress] [nvarchar](80) NULL,
	[RGCity] [nvarchar](40) NULL,
	[RGState] [nvarchar](2) NULL,
	[RGContactPhone] [nvarchar](10) NULL,
	[RGContact] [nvarchar](80) NULL,
	[RGZipCode] [nvarchar](255) NULL,
	[RGContactName] [nvarchar](80) NULL,
	[NumberForms18] [int] NULL,
	[AuthTrans19] [bit] NULL,
	[ALENumberForms20] [int] NULL,
	[ALEGroup21] [bit] NULL,
	[QOfferMethod22A] [bit] NULL,
	[QOfferTranRelief22B] [bit] NULL,
	[Section498022C] [bit] NULL,
	[Offer98Percent22D] [bit] NULL,
	[RTitle] [nvarchar](80) NULL,
	[AllEssential] [nvarchar](1)  NULL,
	[AllFTCount] [int] NULL,
	[AllTotalCount] [int] NULL,
	[AllAggregateGroup] [bit] NULL,
	[All4980Indicator] [nvarchar](10) NULL,
	[JanEssential] [nvarchar](1)  NULL,
	[JanFTCount] [int] NULL,
	[JanTotalCount] [int] NULL,
	[JanAggregateGroup] [bit] NULL,
	[Jan4980Indicator] [nvarchar](10) NULL,
	[FebEssential] [nvarchar](1)  NULL,
	[FebFTCount] [int] NULL,
	[FebTotalCount] [int] NULL,
	[FebAggregateGroup] [bit] NULL,
	[Feb4980Indicator] [nvarchar](10) NULL,
	[MarEssential] [nvarchar](1)  NULL,
	[MarFTCount] [int] NULL,
	[MarTotalCount] [int] NULL,
	[MarAggregateGroup] [bit] NULL,
	[Mar4980Indicator] [nvarchar](10) NULL,
	[AprEssential] [nvarchar](1) NULL,
	[AprFTCount] [int] NULL,
	[AprTotalCount] [int] NULL,
	[AprAggregateGroup] [bit] NULL,
	[Apr4980Indicator] [nvarchar](10) NULL,
	[MayEssential] [nvarchar](1) NULL,
	[MayFTCount] [int] NULL,
	[MayTotalCount] [int] NULL,
	[MayAggregateGroup] [bit] NULL,
	[May4980Indicator] [nvarchar](10) NULL,
	[JunEssential] [nvarchar](1) NULL,
	[JunFTCount] [int] NULL,
	[JunTotalCount] [int] NULL,
	[JunAggregateGroup] [bit] NULL,
	[Jun4980Indicator] [nvarchar](10) NULL,
	[JulEssential] [nvarchar](1) NULL,
	[JulFTCount] [int] NULL,
	[JulTotalCount] [int] NULL,
	[JulAggregateGroup] [bit] NULL,
	[Jul4980Indicator] [nvarchar](10) NULL,
	[AugEssential] [nvarchar](1) NULL,
	[AugFTCount] [int] NULL,
	[AugTotalCount] [int] NULL,
	[AugAggregateGroup] [bit] NULL,
	[Aug4980Indicator] [nvarchar](10) NULL,
	[SepEssential] [nvarchar](1) NULL,
	[SepFTCount] [int] NULL,
	[SepTotalCount] [int] NULL,
	[SepAggregateGroup] [bit] NULL,
	[Sep4980Indicator] [nvarchar](10) NULL,
	[OctEssential] [nvarchar](1) NULL,
	[OctFTCount] [int] NULL,
	[OctTotalCount] [int] NULL,
	[OctAggregateGroup] [bit] NULL,
	[Oct4980Indicator] [nvarchar](10) NULL,
	[NovEssential] [nvarchar](1) NULL,
	[NovFTCount] [int] NULL,
	[NovTotalCount] [int] NULL,
	[NovAggregateGroup] [bit] NULL,
	[Nov4980Indicator] [nvarchar](10) NULL,
	[DecEssential] [nvarchar](1) NULL,
	[DecFTCount] [int] NULL,
	[DecTotalCount] [int] NULL,
	[DecAggregateGroup] [bit] NULL,
	[Dec4980Indicator] [nvarchar](10) NULL,
	[Total1095Count] [smallint] NULL,
	[HTestId] [nvarchar](4) NULL,
	[IRSTransmissionID] [nvarchar](20) NULL,
	[dTime] [timestamp] NULL,
	[StartMonth] [char](2) NULL,
PRIMARY KEY CLUSTERED 
(
	[EmployerFilingBaseId] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_CoveredAll]  DEFAULT ((0)) FOR [CoveredAll]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Jan]  DEFAULT ((0)) FOR [Jan]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Feb]  DEFAULT ((0)) FOR [Feb]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Mar]  DEFAULT ((0)) FOR [Mar]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Apr]  DEFAULT ((0)) FOR [Apr]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_May]  DEFAULT ((0)) FOR [May]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Jun]  DEFAULT ((0)) FOR [Jun]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Jul]  DEFAULT ((0)) FOR [Jul]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Aug]  DEFAULT ((0)) FOR [Aug]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Sep]  DEFAULT ((0)) FOR [Sep]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Oct]  DEFAULT ((0)) FOR [Oct]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Nov]  DEFAULT ((0)) FOR [Nov]
GO
ALTER TABLE [dbo].[YE1095Covered] ADD  CONSTRAINT [DF_YE1095Covered_Dec]  DEFAULT ((0)) FOR [Dec]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [DF_YE1095Head_PartIII_EmployerProvidedSelfIns]  DEFAULT ((0)) FOR [PartIII_EmployerProvidedSelfIns]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [DF_YE1095Head_Corrected]  DEFAULT ((0)) FOR [Corrected]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [DF_YE1095Head_Void]  DEFAULT ((0)) FOR [Void]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [DF_YE1095Head_Frozen]  DEFAULT ((0)) FOR [Frozen]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Jan]  DEFAULT ('1H') FOR [L14_Jan]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Feb]  DEFAULT ('1H') FOR [L14_Feb]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Mar]  DEFAULT ('1H') FOR [L14_Mar]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Apr]  DEFAULT ('1H') FOR [L14_Apr]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_May]  DEFAULT ('1H') FOR [L14_May]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Jun]  DEFAULT ('1H') FOR [L14_Jun]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Jul]  DEFAULT ('1H') FOR [L14_Jul]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Aug]  DEFAULT ('1H') FOR [L14_Aug]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Sep]  DEFAULT ('1H') FOR [L14_Sep]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Oct]  DEFAULT ('1H') FOR [L14_Oct]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Nov]  DEFAULT ('1H') FOR [L14_Nov]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [c_L14_Dec]  DEFAULT ('1H') FOR [L14_Dec]
GO
ALTER TABLE [dbo].[YE1095Head] ADD  CONSTRAINT [DF_YE1095Head_SendRecord]  DEFAULT ((1)) FOR [SendRecord]
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled] ADD  DEFAULT ((0)) FOR [Corrected]
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled] ADD  DEFAULT ((0)) FOR [Frozen]
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled] ADD  DEFAULT ('00') FOR [StartMonth]
GO
ALTER TABLE [dbo].[YE1095Covered]  WITH CHECK ADD  CONSTRAINT [FK_YE1095Covered_YE1095Head] FOREIGN KEY([EmployerFilingBaseId], [IndividualSSN])
REFERENCES [dbo].[YE1095Head] ([EmployerFilingBaseId], [IndividualSSN])
ON UPDATE CASCADE
GO
ALTER TABLE [dbo].[YE1095Covered] CHECK CONSTRAINT [FK_YE1095Covered_YE1095Head]
GO
ALTER TABLE [dbo].[YE1095Head]  WITH CHECK ADD  CONSTRAINT [FK_YE1095Head_YEEmployerBaseFiled] FOREIGN KEY([EmployerFilingBaseId])
REFERENCES [dbo].[YEEmployerBaseFiled] ([EmployerFilingBaseId])
ON UPDATE CASCADE
GO
ALTER TABLE [dbo].[YE1095Head] CHECK CONSTRAINT [FK_YE1095Head_YEEmployerBaseFiled]
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([AllEssential]='N' OR [AllEssential]='Y' OR [AllEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([AprEssential]='N' OR [AprEssential]='Y' OR [AprEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([AugEssential]='N' OR [AugEssential]='Y' OR [AugEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([DecEssential]='N' OR [DecEssential]='Y' OR [DecEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([FebEssential]='N' OR [FebEssential]='Y' OR [FebEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([JanEssential]='N' OR [JanEssential]='Y' OR [JanEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([JulEssential]='N' OR [JulEssential]='Y' OR [JulEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([JunEssential]='N' OR [JunEssential]='Y' OR [JunEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([MarEssential]='N' OR [MarEssential]='Y' OR [MarEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([MayEssential]='N' OR [MayEssential]='Y' OR [MayEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([NovEssential]='N' OR [NovEssential]='Y' OR [NovEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([OctEssential]='N' OR [OctEssential]='Y' OR [OctEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([SepEssential]='N' OR [SepEssential]='Y' OR [SepEssential]='X'))
GO
ALTER TABLE [dbo].[YEEmployerBaseFiled]  WITH CHECK ADD CHECK  (([StartMonth]='12' OR [StartMonth]='11' OR [StartMonth]='10' OR [StartMonth]='09' OR [StartMonth]='08' OR [StartMonth]='07' OR [StartMonth]='06' OR [StartMonth]='05' OR [StartMonth]='04' OR [StartMonth]='03' OR [StartMonth]='02' OR [StartMonth]='01' OR [StartMonth]='00'))
GO