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
-- Table structure for table `alignments`
--

DROP TABLE IF EXISTS `alignments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `alignments` (
  `detectorName` text,
  `detectorID` int(11) DEFAULT NULL,
  `deltaX` double DEFAULT NULL,
  `deltaY` double DEFAULT NULL,
  `deltaZ` double DEFAULT NULL,
  `rotX` double DEFAULT NULL,
  `rotY` double DEFAULT NULL,
  `rotZ` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `alignments`
--

LOCK TABLES `alignments` WRITE;
/*!40000 ALTER TABLE `alignments` DISABLE KEYS */;
INSERT INTO `alignments` VALUES ('D1V',1,0,0,0,0,0,0),('D1Vp',2,0,0,0,0,0,0),('D1X',3,0,0,0,0,0,0),('D1Xp',4,0,0,0,0,0,0),('D1U',5,0,0,0,0,0,0),('D1Up',6,0,0,0,0,0,0),('D2V',7,0,0,0,0,0,0),('D2Vp',8,0,0,0,0,0,0),('D2Xp',9,0,0,0,0,0,0),('D2X',10,0,0,0,0,0,0),('D2U',11,0,0,0,0,0,0),('D2Up',12,0,0,0,0,0,0),('D3pVp',13,0,0,0,0,0,0),('D3pV',14,0,0,0,0,0,0),('D3pXp',15,0,0,0,0,0,0),('D3pX',16,0,0,0,0,0,0),('D3pUp',17,0,0,0,0,0,0),('D3pU',18,0,0,0,0,0,0),('D3mVp',19,0,0,0,0,0,0),('D3mV',20,0,0,0,0,0,0),('D3mXp',21,0,0,0,0,0,0),('D3mX',22,0,0,0,0,0,0),('D3mUp',23,0,0,0,0,0,0),('D3mU',24,0,0,0,0,0,0),('H1B',25,0,0,0,0,0,0),('H1T',26,0,0,0,0,0,0),('H1L',27,0,0,0,0,0,0),('H1R',28,0,0,0,0,0,0),('H2L',29,0,0,0,0,0,0),('H2R',30,0,0,0,0,0,0),('H2B',31,0,0,0,0,0,0),('H2T',32,0,0,0,0,0,0),('H3B',33,0,0,0,0,0,0),('H3T',34,0,0,0,0,0,0),('H4Y1L',35,0,0,0,0,0,0),('H4Y1R',36,0,0,0,0,0,0),('H4Y2L',37,0,0,0,0,0,0),('H4Y2R',38,0,0,0,0,0,0),('H4B',39,0,0,0,0,0,0),('H4T',40,0,0,0,0,0,0),('P1Y1',41,0,0,0,0,0,0),('P1Y2',42,0,0,0,0,0,0),('P1X1',43,0,0,0,0,0,0),('P1X2',44,0,0,0,0,0,0),('P2X1',45,0,0,0,0,0,0),('P2X2',46,0,0,0,0,0,0),('P2Y1',47,0,0,0,0,0,0),('P2Y2',48,0,0,0,0,0,0);
/*!40000 ALTER TABLE `alignments` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `instrumentation`
--

DROP TABLE IF EXISTS `instrumentation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `instrumentation` (
  `instruName` text,
  `radius` double DEFAULT NULL,
  `length` double DEFAULT NULL,
  `xPos` double DEFAULT NULL,
  `yPos` double DEFAULT NULL,
  `zPos` double DEFAULT NULL,
  `material` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `instrumentation`
--

LOCK TABLES `instrumentation` WRITE;
/*!40000 ALTER TABLE `instrumentation` DISABLE KEYS */;
INSERT INTO `instrumentation` VALUES ('CerenkovBody',6.625,31.7,0,0,-855.0304,'ArCO2'),('CerenkovWinF',6.625,0.0254,0,0,-870.8842099999999,'Titanium'),('CerenkovWinB',6.625,0.00762,0,0,-839.17659,'Titanium'),('SWIC',6.625,0.0065,0,0,-836.5169,'Tungsten'),('HeliumBag',6.625,609.6,0,0,-515.7054,'Helium'),('HeliumWinF',6.625,0.0254,0,0,-820.5181,'Titanium'),('HeliumWinB',6.625,0.0254,0,0,-210.89270000000005,'Titanium');
/*!40000 ALTER TABLE `instrumentation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `materials`
--

DROP TABLE IF EXISTS `materials`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `materials` (
  `materialID` double DEFAULT NULL,
  `material` text,
  `density` double DEFAULT NULL,
  `nComponents` int(11) DEFAULT NULL,
  `receipt` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `materials`
--

LOCK TABLES `materials` WRITE;
/*!40000 ALTER TABLE `materials` DISABLE KEYS */;
INSERT INTO `materials` VALUES (1,'LH2',0.07065,1,'(G4_H : 1.)'),(2,'LD2',0.1617,1,'(D : 1.)'),(3,'Carbon',1.802,1,'(G4_C : 1.)'),(4,'Iron',7.874,1,'(G4_Fe : 1.)'),(5,'Tungsten',19.3,1,'(G4_W : 1.)'),(6,'Ammonia',0.917,2,'(G4_N : 0.822, G4_H : 0.178)'),(7,'P08CF4',0.00177504,3,'(G4_Ar : 0.89121, G4_C : 0.03227, G4_H : 0.00717, G4_F : 0.06935'),(8,'Paper',1.42,4,'(G4_C : 0.00505, G4_H : 0.065, G4_O : 0.425, G4_H : 0.005)'),(9,'Vacuum',0.000000000001,2,'(G4_N : 0.7, G4_O : 0.3)'),(10,'ArCO2',0.001822,3,'(G4_Ar : 0.77652, G4_C : 0.05834, G4_O : 0.16514)'),(11,'Titanium',4.54,1,'(G4_Ti : 1.)'),(12,'Helium',0.000166,1,'(G4_He : 1.)'),(13,'Aluminium',2.70000417323161,1,'(G4_Al : 1.)');
/*!40000 ALTER TABLE `materials` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `nondetectors`
--

DROP TABLE IF EXISTS `nondetectors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `nondetectors` (
  `volumeGroupName` text,
  `volumeName` text,
  `xLength` double DEFAULT NULL,
  `yLength` double DEFAULT NULL,
  `zLength` double DEFAULT NULL,
  `xPos` double DEFAULT NULL,
  `yPos` double DEFAULT NULL,
  `zPos` double DEFAULT NULL,
  `material` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `nondetectors`
--

LOCK TABLES `nondetectors` WRITE;
/*!40000 ALTER TABLE `nondetectors` DISABLE KEYS */;
INSERT INTO `nondetectors` VALUES ('Absorber','Absorber',320.04,345.44,99.568,0,0,2028.19,'Iron'),('Fmag','FMagCore',160.02,129.54,502.92,0,0,251.46,'Iron'),('FMag','FMagLeft',160.02,129.54,490.22,-233.68,0,251.46,'Iron'),('FMag','FMagRight',160.02,129.54,490.22,233.68,0,251.46,'Iron'),('FMag','FMagTop',502.92,86.36,480.06,0,107.95,251.46,'Iron'),('FMag','FMagBottom',502.92,86.36,480.06,0,-107.95,251.46,'Iron'),('FMag','FMagCoilLeft',73.66,129.54,490.22,-116.84,0,251.46,'Aluminium'),('FMag','FMagCoilRight',73.66,129.54,490.22,116.84,0,251.46,'Aluminium');
/*!40000 ALTER TABLE `nondetectors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `planes`
--

DROP TABLE IF EXISTS `planes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `planes` (
  `detectorGroupName` text,
  `detectorName` text,
  `detectorID` int(11) DEFAULT NULL,
  `spacing` double DEFAULT NULL,
  `cellWidth` double DEFAULT NULL,
  `overlap` double DEFAULT NULL,
  `numElements` int(11) DEFAULT NULL,
  `angleFromVert` double DEFAULT NULL,
  `xPrimeOffset` double DEFAULT NULL,
  `planeWidth` double DEFAULT NULL,
  `planeHeight` double DEFAULT NULL,
  `x0` double DEFAULT NULL,
  `y0` double DEFAULT NULL,
  `z0` double DEFAULT NULL,
  `theta_x` double DEFAULT NULL,
  `theta_y` double DEFAULT NULL,
  `theta_z` double DEFAULT NULL,
  `material` text
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `planes`
--

LOCK TABLES `planes` WRITE;
/*!40000 ALTER TABLE `planes` DISABLE KEYS */;
INSERT INTO `planes` VALUES ('D1','D1V',1,0.5,0.5,0,384,0.244974,0,152.4,137.16,0,0,666.5,0,0,0,'P08CF4'),('D1','D1Vp',2,0.5,0.5,0,384,0.244974,-0.25,152.4,137.16,0,0,667.1,0,0,0,'P08CF4'),('D1','D1X',3,0.5,0.5,0,320,0,0,152.4,137.16,0,0,667.7,0,0,0,'P08CF4'),('D1','D1Xp',4,0.5,0.5,0,320,0,-0.25,152.4,137.16,0,0,668.3,0,0,0,'P08CF4'),('D1','D1U',5,0.5,0.5,0,384,-0.244974,0,152.4,137.16,0,0,668.9,0,0,0,'P08CF4'),('D1','D1Up',6,0.5,0.5,0,384,-0.244974,-0.25,152.4,137.16,0,0,669.5,0,0,0,'P08CF4'),('D2','D2V',7,2.021,2.021,0,128,-0.244974,-0.505,233.274,264.16,0,0,1314.87,0,0,0,'P08CF4'),('D2','D2Vp',8,2.021,2.021,0,128,-0.244974,0.505,233.274,264.16,0,0,1321.86,0,0,0,'P08CF4'),('D2','D2Xp',9,2.083,2.083,0,112,0,-0.521,233.274,264.16,0,0,1340.26,0,0,0,'P08CF4'),('D2','D2X',10,2.083,2.083,0,112,0,0.521,233.274,264.16,0,0,1347.35,0,0,0,'P08CF4'),('D2','D2U',11,2.021,2.021,0,128,0.244974,-0.505,233.274,264.16,0,0,1365.85,0,0,0,'P08CF4'),('D2','D2Up',12,2.021,2.021,0,128,0.244974,0.505,233.274,264.16,0,0,1372.83,0,0,0,'P08CF4'),('D3p','D3pVp',13,2,2,0,134,0.244974,0.5,320,166,0,78.694,1923.3,0,0,0,'P08CF4'),('D3p','D3pV',14,2,2,0,134,0.244974,-0.5,320,166,0,78.694,1925.3,0,0,0,'P08CF4'),('D3p','D3pXp',15,2,2,0,116,0,0.5,320,166,0,78.694,1929.3,0,0,0,'P08CF4'),('D3p','D3pX',16,2,2,0,116,0,-0.5,320,166,0,78.694,1931.3,0,0,0,'P08CF4'),('D3p','D3pUp',17,2,2,0,134,-0.244974,0.5,320,166,0,78.694,1935.3,0,0,0,'P08CF4'),('D3p','D3pU',18,2,2,0,134,-0.244974,-0.5,320,166,0,78.694,1937.3,0,0,0,'P08CF4'),('D3m','D3mVp',19,2,2,0,134,0.244974,0.5,320,166,0,-79.5883,1886.71,0,0,0,'P08CF4'),('D3m','D3mV',20,2,2,0,134,0.244974,-0.5,320,166,0,-79.5883,1888.71,0,0,0,'P08CF4'),('D3m','D3mXp',21,2,2,0,116,0,0.5,320,166,0,-79.5883,1892.71,0,0,0,'P08CF4'),('D3m','D3mX',22,2,2,0,116,0,-0.5,320,166,0,-79.5883,1894.71,0,0,0,'P08CF4'),('D3m','D3mUp',23,2,2,0,134,-0.244974,0.5,320,166,0,-79.5883,1898.71,0,0,0,'P08CF4'),('D3m','D3mU',24,2,2,0,134,-0.244974,-0.5,320,166,0,-79.5883,1900.71,0,0,0,'P08CF4'),('H1X','H1B',25,7.0025,7.32,0.3175,23,0,0,162.008,69.85,0,-34.925,644,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1X','H1T',26,7.0025,7.32,0.3175,23,0,0,162.008,69.85,0,34.925,644,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1Y','H1L',27,7.0025,7.32,0.3175,20,1.5708,0,78.74,140.117,39.37,0,628,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H1Y','H1R',28,7.0025,7.32,0.3175,20,1.5708,0,78.74,140.117,-39.37,0,628,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2Y','H2L',29,12.6825,13,0.3175,19,1.5708,0,132,241.285,66,0,1405.09,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2Y','H2R',30,12.6825,13,0.3175,19,1.5708,0,132,241.285,-66,0,1404.78,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2X','H2B',31,12.6825,13,0.3175,16,0,0,203.238,152,0,-76,1420.95,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H2X','H2T',32,12.6825,13,0.3175,16,0,0,203.238,152,0,76,1421.28,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H3X','H3B',33,14.27,14.5875,0.3175,16,0,0,227.518,167.64,0,-83.82,1958.34,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H3X','H3T',34,14.27,14.5875,0.3175,16,0,0,227.518,167.64,0,83.82,1958.9,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y1','H4Y1L',35,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,76.2,0,2130.27,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y1','H4Y1R',36,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,-76.2,0,2146.45,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y2','H4Y2L',37,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,76.2,0,2200.44,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4Y2','H4Y2R',38,23.16,23.4775,0.3175,16,1.5708,0,152.4,365.797,-76.2,0,2216.62,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4X','H4B',39,19.33,19.6475,0.3175,16,0,0,304.518,182.88,0,-91.44,2251.71,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('H4X','H4T',40,19.33,19.6475,0.3175,16,0,0,304.518,182.88,0,91.44,2234.29,0,0,0,'G4_PLASTIC_SC_VINYLTOLUENE'),('P1Y','P1Y1',41,5.08,5.08,0,72,1.5708,1.27,365.76,365.76,0,0,2098.13,0,0,0,'P08CF4'),('P1Y','P1Y2',42,5.08,5.08,0,72,1.5708,-1.27,365.76,365.76,0,0,2102.53,0,0,0,'P08CF4'),('P1X','P1X1',43,5.08,5.08,0,72,0,1.27,365.76,365.76,0,0,2174.23,0,0,0,'P08CF4'),('P1X','P1X2',44,5.08,5.08,0,72,0,-1.27,365.76,365.76,0,0,2178.63,0,0,0,'P08CF4'),('P2X','P2X1',45,5.08,5.08,0,72,0,1.27,365.76,365.76,0,0,2367.04,0,0,0,'P08CF4'),('P2X','P2X2',46,5.08,5.08,0,72,0,-1.27,365.76,365.76,0,0,2371.44,0,0,0,'P08CF4'),('P2Y','P2Y1',47,5.08,5.08,0,72,1.5708,1.27,365.76,365.76,0,0,2389.51,0,0,0,'P08CF4'),('P2Y','P2Y2',48,5.08,5.08,0,72,1.5708,-1.27,365.76,365.76,0,0,2393.91,0,0,0,'P08CF4');
/*!40000 ALTER TABLE `planes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `targets`
--

DROP TABLE IF EXISTS `targets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `targets` (
  `targetID` int(11) DEFAULT NULL,
  `targetName` text,
  `nPieces` int(11) DEFAULT NULL,
  `length` double DEFAULT NULL,
  `spacing` double DEFAULT NULL,
  `radius` double DEFAULT NULL,
  `z0` double DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `targets`
--

LOCK TABLES `targets` WRITE;
/*!40000 ALTER TABLE `targets` DISABLE KEYS */;
INSERT INTO `targets` VALUES (1,'LH2',1,50.8,0,3.81,-129.54),(3,'LD2',1,50.8,0,3.81,-129.54),(5,'Iron',3,0.635,17.018,2.54,-129.54),(6,'Carbon',3,1.10744,17.018,2.54,-129.54),(7,'Tungsten',3,0.3175,17.018,2.54,-129.54),(8,'Ammonia',1,8,0,2,-330);
/*!40000 ALTER TABLE `targets` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-11-30 16:41:51
