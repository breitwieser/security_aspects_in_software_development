/* Delete existing database instance */
DROP DATABASE IF EXISTS `sasesubsystem`;
/* Create new empty database */
CREATE DATABASE IF NOT EXISTS `sasesubsystem` CHARACTER SET utf8;

USE `sasesubsystem`;

CREATE TABLE Users (
  `id`    INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL,
  `name`  VARCHAR(20) NOT NULL,
  `email` VARCHAR(60) NOT NULL,
  `pass`  VARCHAR(32) NOT NULL,
  `iter`  INTEGER NOT NULL,
  `salt`  VARCHAR(32) NOT NULL
);

CREATE TABLE Roles (
  `id`   INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL,
  `name` VARCHAR(20) NOT NULL
);

CREATE TABLE UserRoles (
  `uid` INTEGER NOT NULL,
  `rid` INTEGER NOT NULL,
  CONSTRAINT pk_user_roles PRIMARY KEY(`uid`,`rid`),
  FOREIGN KEY (`uid`) REFERENCES Users(`id`),
  FOREIGN KEY (`rid`) REFERENCES Roles(`id`)
);

CREATE TABLE Students (
  `uid`   INTEGER NOT NULL,
  `gid`   INTEGER NOT NULL,
  CONSTRAINT pk_students PRIMARY KEY(`uid`,`gid`),
  FOREIGN KEY (`uid`) REFERENCES Users(`id`)
);

CREATE TABLE Admins (
	`uid`   					INTEGER PRIMARY KEY NOT NULL,
	`configTableRows`	INTEGER NOT NULL,
	`userTableRows`		INTEGER NOT NULL,
	FOREIGN KEY (`uid`) REFERENCES Users(`id`)
);

CREATE TABLE Configuration(
  `id`      INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL,
  `pname`   VARCHAR(40) NOT NULL,
  `pvalue`  TEXT
);

INSERT INTO Configuration(`pname`, `pvalue`) VALUES('PwdSecSpecIter', '4096');
INSERT INTO Configuration(`pname`, `pvalue`) VALUES('CompilerCommand', 'gcc main.c submission.c -o submission');

INSERT INTO Roles(`name`) VALUES('Administrator'), ('Student');
INSERT INTO Users(`name`, `email`, `pass`, `iter`, `salt`) VALUES ('admin','none', '40021cfcffabf4670fff4392faf1ea9e',4096,'a36c310c115388a569cd1b72f238cabc'); /* Password: SASE2013 */
INSERT INTO UserRoles(`uid`,`rid`) SELECT LAST_INSERT_ID(), Roles.`id` FROM Roles WHERE name='Administrator';
INSERT INTO Admins(`uid`, `configTableRows`, `userTableRows`) SELECT Users.`id`, 20, 20 FROM Users WHERE name='admin';  

CREATE VIEW UsersView AS SELECT Users.`name` AS user_name, Users.`pass` AS user_pass FROM Users;
CREATE VIEW UserRolesView AS SELECT Users.name AS user_name, Roles.name AS role_name FROM Users JOIN (UserRoles,Roles) ON UserRoles.`uid`=Users.`id` AND UserRoles.`rid`=Roles.`id`;

COMMIT;
