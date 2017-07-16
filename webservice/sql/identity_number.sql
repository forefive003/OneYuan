/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 60011
Source Host           : localhost:3306
Source Database       : guanyuan

Target Server Type    : MYSQL
Target Server Version : 60011
File Encoding         : 65001

Date: 2015-09-28 23:42:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `identity_number`
-- ----------------------------
DROP TABLE IF EXISTS `identity_number`;
CREATE TABLE `identity_number` (
  `id` bigint(20) NOT NULL,
  `locale` varchar(20) DEFAULT NULL,
  `lsh` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of identity_number
-- ----------------------------
INSERT INTO `identity_number` VALUES ('1', '10', '1f0003');
