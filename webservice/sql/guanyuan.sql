/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50611
Source Host           : localhost:3306
Source Database       : guanyuan

Target Server Type    : MYSQL
Target Server Version : 50611
File Encoding         : 65001

Date: 2015-08-03 16:00:47
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for deviceinfo
-- ----------------------------
DROP TABLE IF EXISTS `deviceinfo`;
CREATE TABLE `deviceinfo` (
  `id` bigint(11) NOT NULL AUTO_INCREMENT,
  `actionIP` varchar(255) DEFAULT NULL,
  `workStationIP` varchar(255) DEFAULT NULL,
  `workStationPort` varchar(11) DEFAULT NULL,
  `cardDeviceName` varchar(255) DEFAULT NULL,
  `encryptIndex` varchar(255) DEFAULT NULL,
  `regTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=74 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deviceinfo
-- ----------------------------

-- ----------------------------
-- Table structure for exception_log
-- ----------------------------
DROP TABLE IF EXISTS `exception_log`;
CREATE TABLE `exception_log` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `taskInfoID` bigint(255) DEFAULT NULL,
  `errorInfo` varchar(255) DEFAULT NULL,
  `errorTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=307 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of exception_log
-- ----------------------------

-- ----------------------------
-- Table structure for globalvalue
-- ----------------------------
DROP TABLE IF EXISTS `globalvalue`;
CREATE TABLE `globalvalue` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `global_value` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of globalvalue
-- ----------------------------
INSERT INTO `globalvalue` VALUES ('1', '1000000001');

-- ----------------------------
-- Table structure for task_info
-- ----------------------------
DROP TABLE IF EXISTS `task_info`;
CREATE TABLE `task_info` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `client_name` varchar(255) DEFAULT NULL,
  `queue_name` varchar(255) DEFAULT NULL,
  `task_name` varchar(255) DEFAULT NULL,
  `task_data` text,
  `intPriority` varchar(20) DEFAULT NULL,
  `taskID` varchar(255) DEFAULT NULL,
  `status` tinyint(4) DEFAULT NULL,
  `addTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=614 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of task_info
-- ----------------------------

-- ----------------------------
-- Table structure for ws_error_log
-- ----------------------------
DROP TABLE IF EXISTS `ws_error_log`;
CREATE TABLE `ws_error_log` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `taskInfoID` bigint(255) DEFAULT NULL,
  `wsType` varchar(255) DEFAULT NULL,
  `isOK` int(255) DEFAULT NULL,
  `SBYY` varchar(255) DEFAULT NULL,
  `errorTime` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=196 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- ----------------------------
-- Records of ws_error_log
-- ----------------------------
