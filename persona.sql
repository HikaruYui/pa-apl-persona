-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: May 08, 2026 at 08:54 AM
-- Server version: 11.4.10-MariaDB-log
-- PHP Version: 8.0.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `persona`
--

-- --------------------------------------------------------

--
-- Table structure for table `arcana_fusion_matrix`
--

CREATE TABLE `arcana_fusion_matrix` (
  `arcana_1` int(11) DEFAULT NULL,
  `arcana_2` int(11) DEFAULT NULL,
  `result_arcana` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `arcana_fusion_matrix`
--

INSERT INTO `arcana_fusion_matrix` (`arcana_1`, `arcana_2`, `result_arcana`) VALUES
(1, 1, 1),
(1, 2, 10),
(1, 3, 6),
(1, 4, 5),
(1, 5, 9),
(1, 6, 2),
(1, 7, 8),
(1, 8, 3),
(1, 9, 4),
(1, 10, 7),
(2, 1, 10),
(2, 2, 2),
(2, 3, 7),
(2, 4, 9),
(2, 5, 8),
(2, 6, 1),
(2, 7, 3),
(2, 8, 4),
(2, 9, 5),
(2, 10, 6),
(3, 1, 6),
(3, 2, 7),
(3, 3, 3),
(3, 4, 2),
(3, 5, 10),
(3, 6, 9),
(3, 7, 1),
(3, 8, 5),
(3, 9, 8),
(3, 10, 4),
(4, 1, 5),
(4, 2, 9),
(4, 3, 2),
(4, 4, 4),
(4, 5, 3),
(4, 6, 8),
(4, 7, 10),
(4, 8, 6),
(4, 9, 7),
(4, 10, 1),
(5, 1, 9),
(5, 2, 8),
(5, 3, 10),
(5, 4, 3),
(5, 5, 5),
(5, 6, 7),
(5, 7, 2),
(5, 8, 1),
(5, 9, 6),
(5, 10, 4),
(6, 1, 2),
(6, 2, 1),
(6, 3, 9),
(6, 4, 8),
(6, 5, 7),
(6, 6, 6),
(6, 7, 4),
(6, 8, 10),
(6, 9, 3),
(6, 10, 5),
(7, 1, 8),
(7, 2, 3),
(7, 3, 1),
(7, 4, 10),
(7, 5, 2),
(7, 6, 4),
(7, 7, 7),
(7, 8, 9),
(7, 9, 5),
(7, 10, 6),
(8, 1, 3),
(8, 2, 4),
(8, 3, 5),
(8, 4, 6),
(8, 5, 1),
(8, 6, 10),
(8, 7, 9),
(8, 8, 8),
(8, 9, 2),
(8, 10, 7),
(9, 1, 4),
(9, 2, 5),
(9, 3, 8),
(9, 4, 7),
(9, 5, 6),
(9, 6, 3),
(9, 7, 5),
(9, 8, 2),
(9, 9, 9),
(9, 10, 1),
(10, 1, 7),
(10, 2, 6),
(10, 3, 4),
(10, 4, 1),
(10, 5, 4),
(10, 6, 5),
(10, 7, 6),
(10, 8, 7),
(10, 9, 1),
(10, 10, 10);

-- --------------------------------------------------------

--
-- Table structure for table `arcana_master`
--

CREATE TABLE `arcana_master` (
  `id` int(11) NOT NULL,
  `nama_arcana` varchar(25) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `arcana_master`
--

INSERT INTO `arcana_master` (`id`, `nama_arcana`) VALUES
(1, 'Fool'),
(2, 'Magician'),
(3, 'Priestess'),
(4, 'Empress'),
(5, 'Emperor'),
(6, 'Hierophant'),
(7, 'Lovers'),
(8, 'Chariot'),
(9, 'Justice'),
(10, 'Death');

-- --------------------------------------------------------

--
-- Table structure for table `inventori_user`
--

CREATE TABLE `inventori_user` (
  `id` int(11) NOT NULL,
  `user_id` int(11) DEFAULT NULL,
  `skill_tersimpan` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `persona`
--

CREATE TABLE `persona` (
  `id` int(11) NOT NULL,
  `nama` varchar(50) DEFAULT NULL,
  `level` int(11) DEFAULT NULL,
  `harga` int(11) NOT NULL DEFAULT 100,
  `isSpecial` tinyint(1) NOT NULL DEFAULT 0,
  `arcana_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `persona`
--

INSERT INTO `persona` (`id`, `nama`, `level`, `harga`, `isSpecial`, `arcana_id`) VALUES
(1, 'Orpheus', 10, 1000, 0, 1),
(2, 'Slime', 12, 1200, 0, 1),
(3, 'Legion', 27, 2700, 0, 1),
(4, 'Nekomata', 12, 1200, 0, 2),
(5, 'Jack Frost', 15, 1500, 0, 2),
(6, 'Jack-o\' Lantern', 20, 2000, 0, 2),
(7, 'Apsaras', 11, 1100, 0, 3),
(8, 'Unicorn', 18, 1800, 0, 3),
(9, 'High Pixie', 27, 2700, 0, 3),
(10, 'Leanan Sidhe', 22, 2200, 0, 4),
(11, 'Yakshini', 35, 3500, 0, 4),
(12, 'Hariti', 48, 4800, 0, 4),
(13, 'Forneus', 14, 1400, 0, 5),
(14, 'Oberon', 20, 2000, 0, 5),
(15, 'Take-Mikazuchi', 27, 2700, 0, 5),
(16, 'Omoikane', 13, 1300, 0, 6),
(17, 'Berith', 19, 1900, 0, 6),
(18, 'Shiisaa', 30, 3000, 0, 6),
(19, 'Pixie', 13, 1300, 0, 7),
(20, 'Silky', 16, 1600, 0, 7),
(21, 'Tam Lin', 25, 2500, 0, 7),
(22, 'Ara Mitama', 18, 1800, 0, 8),
(23, 'Chimera', 21, 2100, 0, 8),
(24, 'Zouchouten', 27, 2700, 0, 8),
(25, 'Angel', 13, 1300, 0, 9),
(26, 'Archangel', 20, 2000, 0, 9),
(27, 'Principality', 27, 2700, 0, 9),
(28, 'Pisaca', 26, 2600, 0, 10),
(29, 'Loa', 43, 4300, 0, 10),
(30, 'Samael', 52, 5200, 0, 10);

-- --------------------------------------------------------

--
-- Table structure for table `persona_skills`
--

CREATE TABLE `persona_skills` (
  `persona_id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `persona_skills`
--

INSERT INTO `persona_skills` (`persona_id`, `skill_id`) VALUES
(1, 1),
(4, 1),
(6, 1),
(17, 1),
(23, 1),
(9, 2),
(3, 3),
(18, 3),
(1, 4),
(2, 4),
(13, 4),
(22, 4),
(24, 4),
(20, 5),
(23, 5),
(11, 6),
(5, 7),
(7, 7),
(13, 7),
(20, 7),
(24, 8),
(28, 9),
(1, 10),
(8, 10),
(16, 10),
(19, 10),
(20, 10),
(10, 11),
(15, 12),
(28, 13),
(2, 14),
(11, 15),
(15, 15),
(25, 16),
(8, 17),
(27, 17),
(27, 18),
(8, 19),
(26, 19),
(27, 19),
(29, 20),
(5, 21),
(9, 21),
(18, 21),
(29, 23),
(30, 23),
(14, 24),
(15, 24),
(12, 25),
(9, 26),
(10, 26),
(14, 26),
(12, 27),
(30, 28),
(28, 29),
(29, 30),
(6, 31),
(10, 32),
(30, 33),
(4, 34),
(21, 34),
(23, 34),
(6, 35),
(22, 35),
(3, 36),
(5, 36),
(19, 36),
(2, 37),
(17, 38),
(18, 38),
(22, 38),
(4, 39),
(3, 40),
(16, 40),
(12, 41),
(11, 42),
(17, 43),
(25, 43),
(26, 43),
(21, 44),
(25, 44),
(7, 45),
(13, 46),
(14, 47),
(16, 47),
(19, 47),
(21, 47),
(24, 47),
(7, 48),
(26, 48);

-- --------------------------------------------------------

--
-- Table structure for table `skill_card_shop`
--

CREATE TABLE `skill_card_shop` (
  `id` int(11) NOT NULL,
  `skill_id` int(11) NOT NULL,
  `harga` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `skill_card_shop`
--

INSERT INTO `skill_card_shop` (`id`, `skill_id`, `harga`) VALUES
(1, 31, 2000),
(2, 21, 2000),
(3, 24, 2000),
(4, 22, 3500),
(5, 23, 4500),
(6, 10, 800),
(7, 26, 2500),
(8, 27, 8000),
(9, 4, 500),
(10, 34, 1500),
(11, 15, 3000),
(12, 6, 4000),
(13, 3, 2500),
(14, 37, 1800),
(15, 46, 1500),
(16, 38, 1500),
(17, 44, 1500),
(18, 39, 1800),
(19, 45, 1800),
(20, 28, 3000),
(21, 36, 1000),
(22, 5, 1200),
(23, 14, 800),
(24, 33, 2000),
(25, 32, 800),
(26, 25, 2500),
(27, 9, 3000),
(28, 40, 1500),
(29, 35, 1000),
(30, 8, 2500),
(31, 12, 2000);

-- --------------------------------------------------------

--
-- Table structure for table `skill_master`
--

CREATE TABLE `skill_master` (
  `id` int(11) NOT NULL,
  `nama_skill` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `skill_master`
--

INSERT INTO `skill_master` (`id`, `nama_skill`) VALUES
(1, 'agi'),
(2, 'arrow rain'),
(3, 'assault dive'),
(4, 'bash'),
(5, 'bewilder'),
(6, 'blade of fury'),
(7, 'bufu'),
(8, 'crit rate boost'),
(9, 'dekaja'),
(10, 'dia'),
(11, 'diarama'),
(12, 'elec break'),
(13, 'eiha'),
(14, 'evil touch'),
(15, 'fatal end'),
(16, 'garu'),
(17, 'hama'),
(18, 'holy arrow'),
(19, 'kouha'),
(20, 'life drain'),
(21, 'mabufu'),
(22, 'maeiha'),
(23, 'maeiga'),
(24, 'mazio'),
(25, 'me patra'),
(26, 'media'),
(27, 'mediarama'),
(28, 'masukunda'),
(29, 'mudo'),
(30, 'mamudo'),
(31, 'maragi'),
(32, 'patra'),
(33, 'poison mist'),
(34, 'power slash'),
(35, 'provoke'),
(36, 'pulinpa'),
(37, 'raging tiger'),
(38, 'rakukaja'),
(39, 'rakunda'),
(40, 'rebellion'),
(41, 'samarecarn'),
(42, 'sexy dance'),
(43, 'single shot'),
(44, 'sukukaja'),
(45, 'sukunda'),
(46, 'tarukaja'),
(47, 'zio'),
(48, 'baisudi');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `nama_user` varchar(50) DEFAULT NULL,
  `uang` decimal(9,0) DEFAULT NULL,
  `password` varchar(50) DEFAULT NULL,
  `status` enum('user','admin') DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`id`, `nama_user`, `uang`, `password`, `status`) VALUES
(1, 'ozora', 30000, '038', 'user'),
(2, 'igor', 999999, '4444', 'admin');

-- --------------------------------------------------------

--
-- Table structure for table `user_persona_collection`
--

CREATE TABLE `user_persona_collection` (
  `user_id` int(11) DEFAULT NULL,
  `persona_instance_id` int(11) NOT NULL,
  `original_persona_id` int(11) DEFAULT NULL,
  `level_saat_ini` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `user_persona_equipped_skills`
--

CREATE TABLE `user_persona_equipped_skills` (
  `persona_instance_id` int(11) NOT NULL,
  `skill_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `arcana_master`
--
ALTER TABLE `arcana_master`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `inventori_user`
--
ALTER TABLE `inventori_user`
  ADD PRIMARY KEY (`id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `skill_tersimpan` (`skill_tersimpan`);

--
-- Indexes for table `persona`
--
ALTER TABLE `persona`
  ADD PRIMARY KEY (`id`),
  ADD KEY `arcana_id` (`arcana_id`);

--
-- Indexes for table `persona_skills`
--
ALTER TABLE `persona_skills`
  ADD PRIMARY KEY (`persona_id`,`skill_id`),
  ADD KEY `skill_id` (`skill_id`);

--
-- Indexes for table `skill_card_shop`
--
ALTER TABLE `skill_card_shop`
  ADD PRIMARY KEY (`id`),
  ADD KEY `skill_id` (`skill_id`);

--
-- Indexes for table `skill_master`
--
ALTER TABLE `skill_master`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `nama_user` (`nama_user`);

--
-- Indexes for table `user_persona_collection`
--
ALTER TABLE `user_persona_collection`
  ADD PRIMARY KEY (`persona_instance_id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `original_persona_id` (`original_persona_id`);

--
-- Indexes for table `user_persona_equipped_skills`
--
ALTER TABLE `user_persona_equipped_skills`
  ADD PRIMARY KEY (`persona_instance_id`),
  ADD KEY `skill_id` (`skill_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `arcana_master`
--
ALTER TABLE `arcana_master`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `inventori_user`
--
ALTER TABLE `inventori_user`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `persona`
--
ALTER TABLE `persona`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=31;

--
-- AUTO_INCREMENT for table `skill_card_shop`
--
ALTER TABLE `skill_card_shop`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=32;

--
-- AUTO_INCREMENT for table `skill_master`
--
ALTER TABLE `skill_master`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=49;

--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `user_persona_collection`
--
ALTER TABLE `user_persona_collection`
  MODIFY `persona_instance_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `inventori_user`
--
ALTER TABLE `inventori_user`
  ADD CONSTRAINT `inventori_user_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`),
  ADD CONSTRAINT `inventori_user_ibfk_2` FOREIGN KEY (`skill_tersimpan`) REFERENCES `skill_master` (`id`);

--
-- Constraints for table `persona`
--
ALTER TABLE `persona`
  ADD CONSTRAINT `persona_ibfk_1` FOREIGN KEY (`arcana_id`) REFERENCES `arcana_master` (`id`);

--
-- Constraints for table `persona_skills`
--
ALTER TABLE `persona_skills`
  ADD CONSTRAINT `persona_skills_ibfk_1` FOREIGN KEY (`persona_id`) REFERENCES `persona` (`id`),
  ADD CONSTRAINT `persona_skills_ibfk_2` FOREIGN KEY (`skill_id`) REFERENCES `skill_master` (`id`);

--
-- Constraints for table `skill_card_shop`
--
ALTER TABLE `skill_card_shop`
  ADD CONSTRAINT `skill_card_shop_ibfk_1` FOREIGN KEY (`skill_id`) REFERENCES `skill_master` (`id`);

--
-- Constraints for table `user_persona_collection`
--
ALTER TABLE `user_persona_collection`
  ADD CONSTRAINT `user_persona_collection_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`),
  ADD CONSTRAINT `user_persona_collection_ibfk_2` FOREIGN KEY (`original_persona_id`) REFERENCES `persona` (`id`);

--
-- Constraints for table `user_persona_equipped_skills`
--
ALTER TABLE `user_persona_equipped_skills`
  ADD CONSTRAINT `user_persona_equipped_skills_ibfk_1` FOREIGN KEY (`persona_instance_id`) REFERENCES `user_persona_collection` (`persona_instance_id`),
  ADD CONSTRAINT `user_persona_equipped_skills_ibfk_2` FOREIGN KEY (`skill_id`) REFERENCES `skill_master` (`id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
