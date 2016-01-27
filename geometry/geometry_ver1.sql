-- MySQL dump 10.13  Distrib 5.7.9, for osx10.11 (x86_64)
--
-- Host: localhost    Database: geometry_ver1
-- ------------------------------------------------------
-- Server version	5.7.9

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Alignments`
--

DROP TABLE IF EXISTS `Alignments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Alignments` (
  `detectorName` varchar(5) NOT NULL,
  `detectorID` int(11) NOT NULL,
  `deltaX` int(11) NOT NULL,
  `deltaY` int(11) NOT NULL,
  `deltaZ` int(11) NOT NULL,
  `rotX` int(11) NOT NULL,
  `rotY` int(11) NOT NULL,
  `rotZ` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Alignments`
--

LOCK TABLES `Alignments` WRITE;
/*!40000 ALTER TABLE `Alignments` DISABLE KEYS */;
INSERT INTO `Alignments` VALUES ('D1V',1,0,0,0,0,0,0),('D1Vp',2,0,0,0,0,0,0),('D1X',3,0,0,0,0,0,0),('D1Xp',4,0,0,0,0,0,0),('D1U',5,0,0,0,0,0,0),('D1Up',6,0,0,0,0,0,0),('D2V',7,0,0,0,0,0,0),('D2Vp',8,0,0,0,0,0,0),('D2Xp',9,0,0,0,0,0,0),('D2X',10,0,0,0,0,0,0),('D2U',11,0,0,0,0,0,0),('D2Up',12,0,0,0,0,0,0),('D3pVp',13,0,0,0,0,0,0),('D3pV',14,0,0,0,0,0,0),('D3pXp',15,0,0,0,0,0,0),('D3pX',16,0,0,0,0,0,0),('D3pUp',17,0,0,0,0,0,0),('D3pU',18,0,0,0,0,0,0),('D3mVp',19,0,0,0,0,0,0),('D3mV',20,0,0,0,0,0,0),('D3mXp',21,0,0,0,0,0,0),('D3mX',22,0,0,0,0,0,0),('D3mUp',23,0,0,0,0,0,0),('D3mU',24,0,0,0,0,0,0),('H1B',25,0,0,0,0,0,0),('H1T',26,0,0,0,0,0,0),('H1L',27,0,0,0,0,0,0),('H1R',28,0,0,0,0,0,0),('H2L',29,0,0,0,0,0,0),('H2R',30,0,0,0,0,0,0),('H2B',31,0,0,0,0,0,0),('H2T',32,0,0,0,0,0,0),('H3B',33,0,0,0,0,0,0),('H3T',34,0,0,0,0,0,0),('H4Y1L',35,0,0,0,0,0,0),('H4Y1R',36,0,0,0,0,0,0),('H4Y2L',37,0,0,0,0,0,0),('H4Y2R',38,0,0,0,0,0,0),('H4B',39,0,0,0,0,0,0),('H4T',40,0,0,0,0,0,0),('P1Y1',41,0,0,0,0,0,0),('P1Y2',42,0,0,0,0,0,0),('P1X1',43,0,0,0,0,0,0),('P1X2',44,0,0,0,0,0,0),('P2X1',45,0,0,0,0,0,0),('P2X2',46,0,0,0,0,0,0),('P2Y1',47,0,0,0,0,0,0),('P2Y2',48,0,0,0,0,0,0);
/*!40000 ALTER TABLE `Alignments` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Instrumentation`
--

DROP TABLE IF EXISTS `Instrumentation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Instrumentation` (
  `instruName` varchar(14) NOT NULL,
  `radius` float NOT NULL,
  `length` float NOT NULL,
  `xPos` int(11) NOT NULL,
  `yPos` int(11) NOT NULL,
  `zPos` float NOT NULL,
  `material` varchar(8) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Instrumentation`
--

LOCK TABLES `Instrumentation` WRITE;
/*!40000 ALTER TABLE `Instrumentation` DISABLE KEYS */;
INSERT INTO `Instrumentation` VALUES ('I_CerenkovBody',6.625,31.7,0,0,-855.03,'ArCO2'),('I_CerenkovWinF',6.625,0.0254,0,0,-870.884,'Titanium'),('I_CerenkovWinB',6.625,0.00762,0,0,-839.177,'Titanium'),('I_SWIC',6.625,0.0065,0,0,-836.517,'Tungsten'),('I_HeliumBag1',6.625,486.5,0,0,-577.25,'Helium'),('I_HeliumBag2',6.625,115.1,0,0,-268.45,'Helium'),('I_HeliumWinF',6.625,0.0254,0,0,-820.513,'Titanium'),('I_HeliumWinB',6.625,0.0254,0,0,-210.887,'Titanium');
/*!40000 ALTER TABLE `Instrumentation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Materials`
--

DROP TABLE IF EXISTS `Materials`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Materials` (
  `materialID` int(11) NOT NULL,
  `material` varchar(9) NOT NULL,
  `density` float NOT NULL,
  `nComponents` int(11) NOT NULL,
  `receipt` varchar(64) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Materials`
--

LOCK TABLES `Materials` WRITE;
/*!40000 ALTER TABLE `Materials` DISABLE KEYS */;
INSERT INTO `Materials` VALUES (1,'LH2',0.07065,1,'(G4_H : 1.)'),(2,'LD2',0.1617,1,'(D : 1.)'),(3,'Carbon',1.802,1,'(G4_C : 1.)'),(4,'Iron',7.874,1,'(G4_Fe : 1.)'),(5,'Tungsten',19.3,1,'(G4_W : 1.)'),(6,'Ammonia',0.917,2,'(G4_N : 0.822, G4_H : 0.178)'),(7,'P08CF4',0.00177504,3,'(G4_Ar : 0.89121, G4_C : 0.03227, G4_H : 0.00717, G4_F : 0.06935'),(8,'Paper',1.42,4,'(G4_C : 0.505, G4_H : 0.065, G4_O : 0.425, G4_H : 0.005)'),(9,'Vacuum',0.000000000001,2,'(G4_N : 0.7, G4_O : 0.3)'),(10,'ArCO2',0.001822,3,'(G4_Ar : 0.77652, G4_C : 0.05834, G4_O : 0.16514)'),(11,'Titanium',4.54,1,'(G4_Ti : 1.)'),(12,'Helium',0.000166,1,'(G4_He : 1.)'),(13,'Aluminium',2.7,1,'(G4_Al : 1.)');
/*!40000 ALTER TABLE `Materials` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Nondetectors`
--

DROP TABLE IF EXISTS `Nondetectors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Nondetectors` (
  `volumeGroupName` varchar(8) NOT NULL,
  `volumeName` varchar(13) NOT NULL,
  `xLength` float NOT NULL,
  `yLength` float NOT NULL,
  `zLength` float NOT NULL,
  `xPos` float NOT NULL,
  `yPos` float NOT NULL,
  `zPos` float NOT NULL,
  `material` varchar(9) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Nondetectors`
--

LOCK TABLES `Nondetectors` WRITE;
/*!40000 ALTER TABLE `Nondetectors` DISABLE KEYS */;
INSERT INTO `Nondetectors` VALUES ('Absorber','Absorber',320.04,345.44,99.568,0,0,2028.19,'Iron'),('Fmag','D_FMagCore',160.02,129.54,502.92,0,0,251.46,'Iron'),('FMag','FMagLeft',160.02,129.54,490.22,-233.68,0,251.46,'Iron'),('FMag','FMagRight',160.02,129.54,490.22,233.68,0,251.46,'Iron'),('FMag','FMagTop',502.92,86.36,480.06,0,107.95,251.46,'Iron'),('FMag','FMagBottom',502.92,86.36,480.06,0,-107.95,251.46,'Iron'),('FMag','FMagCoilLeft',73.66,129.54,490.22,-116.84,0,251.46,'Aluminium'),('FMag','FMagCoilRight',73.66,129.54,490.22,116.84,0,251.46,'Aluminium');
/*!40000 ALTER TABLE `Nondetectors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Planes`
--

DROP TABLE IF EXISTS `Planes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Planes` (
  `detectorGroupName` varchar(4) NOT NULL,
  `detectorName` varchar(5) NOT NULL,
  `detectorID` int(11) NOT NULL,
  `triggerLv` int(11) NOT NULL,
  `spacing` float NOT NULL,
  `cellWidth` float NOT NULL,
  `overlap` float NOT NULL,
  `numElements` int(11) NOT NULL,
  `angleFromVert` float NOT NULL,
  `xPrimeOffset` float NOT NULL,
  `planeWidth` float NOT NULL,
  `planeHeight` float NOT NULL,
  `x0` float NOT NULL,
  `y0` float NOT NULL,
  `z0` float NOT NULL,
  `theta_x` int(11) NOT NULL,
  `theta_y` int(11) NOT NULL,
  `theta_z` int(11) NOT NULL,
  `material` varchar(26) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Planes`
--

LOCK TABLES `Planes` WRITE;
/*!40000 ALTER TABLE `Planes` DISABLE KEYS */;
INSERT INTO `Planes` VALUES ('D1','D1V',1,-1,0.5,0.5,0,384,0.244974,0,152.4,137.16,0,0,666.5,0,0,0,'P08CF4'),('D1','D1Vp',2,-1,0.5,0.5,0,384,0.244974,-0.25,152.4,137.16,0,0,667.1,0,0,0,'P08CF4'),('D1','D1X',3,-1,0.5,0.5,0,320,0,0,152.4,137.16,0,0,667.7,0,0,0,'P08CF4'),('D1','D1Xp',4,-1,0.5,0.5,0,320,0,-0.25,152.4,137.16,0,0,668.3,0,0,0,'P08CF4'),('D1','D1U',5,-1,0.5,0.5,0,384,-0.244974,0,152.4,137.16,0,0,668.9,0,0,0,'P08CF4'),('D1','D1Up',6,-1,0.5,0.5,0,384,-0.244974,-0.25,152.4,137.16,0,0,669.5,0,0,0,'P08CF4'),('D2','D2V',7,-1,2.021,2.021,0,128,-0.244974,-0.505,233.274,264.16,0,0,1314.87,0,0,0,'P08CF4'),('D2','D2Vp',8,-1,2.021,2.021,0,128,-0.244974,0.505,233.274,264.16,0,0,1321.86,0,0,0,'P08CF4'),('D2','D2Xp',9,-1,2.083,2.083,0,112,0,-0.521,233.274,264.16,0,0,1340.26,0,0,0,'P08CF4'),('D2','D2X',10,-1,2.083,2.083,0,112,0,0.521,233.274,264.16,0,0,1347.35,0,0,0,'P08CF4'),('D2','D2U',11,-1,2.021,2.021,0,128,0.244974,-0.505,233.274,264.16,0,0,1365.85,0,0,0,'P08CF4'),('D2','D2Up',12,-1,2.021,2.021,0,128,0.244974,0.505,233.274,264.16,0,0,1372.83,0,0,0,'P08CF4'),('D3p','D3pVp',13,-1,2,2,0,134,0.244974,0.5,320,166,0,78.694,1923.3,0,0,0,'P08CF4'),('D3p','D3pV',14,-1,2,2,0,134,0.244974,-0.5,320,166,0,78.694,1925.3,0,0,0,'P08CF4'),('D3p','D3pXp',15,-1,2,2,0,116,0,0.5,320,166,0,78.694,1929.3,0,0,0,'P08CF4'),('D3p','D3pX',16,-1,2,2,0,116,0,-0.5,320,166,0,78.694,1931.3,0,0,0,'P08CF4'),('D3p','D3pUp',17,-1,2,2,0,134,-0.244974,0.5,320,166,0,78.694,1935.3,0,0,0,'P08CF4'),('D3p','D3pU',18,-1,2,2,0,134,-0.244974,-0.5,320,166,0,78.694,1937.3,0,0,0,'P08CF4'),('D3m','D3mVp',19,-1,2,2,0,134,0.244974,0.5,320,166,0,-79.5883,1886.71,0,0,0,'P08CF4'),('D3m','D3mV',20,-1,2,2,0,134,0.244974,-0.5,320,166,0,-79.5883,1888.71,0,0,0,'P08CF4'),('D3m','D3mXp',21,-1,2,2,0,116,0,0.5,320,166,0,-79.5883,1892.71,0,0,0,'P08CF4'),('D3m','D3mX',22,-1,2,2,0,116,0,-0.5,320,166,0,-79.5883,1894.71,0,0,0,'P08CF4'),('D3m','D3mUp',23,-1,2,2,0,134,-0.244974,0.5,320,166,0,-79.5883,1898.71,0,0,0,'P08CF4'),('D3m','D3mU',24,-1,2,2,0,134,-0.244974,-0.5,320,166,0,-79.5883,1900.71,0,0,0,'P08CF4'),('H1X','H1B',25,0,7.0025,7.32,0.3175,23,0,0,162.008,69.85,0,-34.925,644,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1X','H1T',26,0,7.0025,7.32,0.3175,23,0,0,162.008,69.85,0,34.925,644,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1Y','H1L',27,-1,7.0025,7.32,0.3175,20,1.5708,0,78.74,140.117,39.37,0,628,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1Y','H1R',28,-1,7.0025,7.32,0.3175,20,1.5708,0,78.74,140.117,-39.37,0,628,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2Y','H2L',29,-1,12.6825,13,0.3175,19,1.5708,0,132,241.285,66,0,1405.09,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2Y','H2R',30,-1,12.6825,13,0.3175,19,1.5708,0,132,241.285,-66,0,1404.78,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2X','H2B',31,1,12.6825,13,0.3175,16,0,0,203.238,152,0,-76,1420.95,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2X','H2T',32,1,12.6825,13,0.3175,16,0,0,203.238,152,0,76,1421.28,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H3X','H3B',33,2,14.27,14.5875,0.3175,16,0,0,227.518,167.64,0,-83.82,1958.34,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H3X','H3T',34,2,14.27,14.5875,0.3175,16,0,0,227.518,167.64,0,83.82,1958.9,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y1','H4Y1L',35,-1,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,76.2,0,2130.27,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y1','H4Y1R',36,-1,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,-76.2,0,2146.45,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y2','H4Y2L',37,-1,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,76.2,0,2200.44,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y2','H4Y2R',38,-1,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,-76.2,0,2216.62,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4X','H4B',39,3,19.33,19.6475,0.3175,16,0,0,304.518,182.88,0,-91.44,2251.71,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4X','H4T',40,3,19.33,19.6475,0.3175,16,0,0,304.518,182.88,0,91.44,2234.29,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('P1Y','P1Y1',41,-1,5.08,5.08,0,72,1.5708,1.27,365.76,365.76,0,0,2098.13,0,0,0,'P08CF4'),('P1Y','P1Y2',42,-1,5.08,5.08,0,72,1.5708,-1.27,365.76,365.76,0,0,2102.53,0,0,0,'P08CF4'),('P1X','P1X1',43,-1,5.08,5.08,0,72,0,1.27,365.76,365.76,0,0,2174.23,0,0,0,'P08CF4'),('P1X','P1X2',44,-1,5.08,5.08,0,72,0,-1.27,365.76,365.76,0,0,2178.63,0,0,0,'P08CF4'),('P2X','P2X1',45,-1,5.08,5.08,0,72,0,1.27,365.76,365.76,0,0,2367.04,0,0,0,'P08CF4'),('P2X','P2X2',46,-1,5.08,5.08,0,72,0,-1.27,365.76,365.76,0,0,2371.44,0,0,0,'P08CF4'),('P2Y','P2Y1',47,-1,5.08,5.08,0,72,1.5708,1.27,365.76,365.76,0,0,2389.51,0,0,0,'P08CF4'),('P2Y','P2Y2',48,-1,5.08,5.08,0,72,1.5708,-1.27,365.76,365.76,0,0,2393.91,0,0,0,'P08CF4');
/*!40000 ALTER TABLE `Planes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Targets`
--

DROP TABLE IF EXISTS `Targets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Targets` (
  `targetID` int(11) NOT NULL,
  `targetName` varchar(10) NOT NULL,
  `material` varchar(8) NOT NULL,
  `nPieces` int(11) NOT NULL,
  `length` float NOT NULL,
  `spacing` float NOT NULL,
  `radius` float NOT NULL,
  `z0` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Targets`
--

LOCK TABLES `Targets` WRITE;
/*!40000 ALTER TABLE `Targets` DISABLE KEYS */;
INSERT INTO `Targets` VALUES (1,'T_LH2','LH2',1,50.8,0,3.81,-129.54),(3,'T_LD2','LD2',1,50.8,0,3.81,-129.54),(5,'T_Iron','Iron',3,0.635,17.018,2.54,-129.54),(6,'T_Carbon','Carbon',3,1.10744,17.018,2.54,-129.54),(7,'T_Tungsten','Tungsten',3,0.3175,17.018,2.54,-129.54),(8,'T_Ammonia','Ammonia',1,8,0,2,-330);
/*!40000 ALTER TABLE `Targets` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2016-01-27  3:08:13
