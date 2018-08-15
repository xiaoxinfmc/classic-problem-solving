Create table If Not Exists Trips (Id int, Client_Id int, Driver_Id int, City_Id int, Status ENUM('completed', 'cancelled_by_driver', 'cancelled_by_client'), Request_at varchar(50));
Create table If Not Exists Users (Users_Id int, Banned varchar(50), Role ENUM('client', 'driver', 'partner'));
Truncate table Trips;
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('1', '1', '10', '1', 'completed', '2013-10-01');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('2', '2', '11', '1', 'cancelled_by_driver', '2013-10-01');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('3', '3', '12', '6', 'completed', '2013-10-01');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('4', '4', '13', '6', 'cancelled_by_client', '2013-10-01');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('5', '1', '10', '1', 'completed', '2013-10-02');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('6', '2', '11', '6', 'completed', '2013-10-02');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('7', '3', '12', '6', 'completed', '2013-10-02');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('8', '2', '12', '12', 'completed', '2013-10-03');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('9', '3', '10', '12', 'completed', '2013-10-03');
insert into Trips (Id, Client_Id, Driver_Id, City_Id, Status, Request_at) values ('10', '4', '13', '12', 'cancelled_by_driver', '2013-10-03');
Truncate table Users;
insert into Users (Users_Id, Banned, Role) values ('1', 'No', 'client');
insert into Users (Users_Id, Banned, Role) values ('2', 'Yes', 'client');
insert into Users (Users_Id, Banned, Role) values ('3', 'No', 'client');
insert into Users (Users_Id, Banned, Role) values ('4', 'No', 'client');
insert into Users (Users_Id, Banned, Role) values ('10', 'No', 'driver');
insert into Users (Users_Id, Banned, Role) values ('11', 'No', 'driver');
insert into Users (Users_Id, Banned, Role) values ('12', 'No', 'driver');
insert into Users (Users_Id, Banned, Role) values ('13', 'No', 'driver');

select x2.request_at as Day, FORMAT(COALESCE((x1.cancel / x2.total), 0.00), 2) as 'Cancellation Rate'from (
  select count(*) as cancel, status, request_at from (
    select * from (select client_id, driver_id, status, request_at from trips where request_at >= '2013-10-01' and request_at <= '2013-10-03') as t1
    join users on (t1.client_id = users.users_id) where users.banned = 'No' and users.role = 'client'
  ) as t2
  join users on (t2.driver_id = users.users_id)
  where users.banned = 'No' and users.role = 'driver' and status <> 'completed' group by request_at
) as x1
right join (
  select count(*) as total, request_at, status from (
    select * from (select client_id, driver_id, status, request_at from trips where request_at >= '2013-10-01' and request_at <= '2013-10-03') as t3
    join users on (t3.client_id = users.users_id) where users.banned = 'No' and users.role = 'client'
  ) as t4
  join users on (t4.driver_id = users.users_id)
  where users.banned = 'No' and users.role = 'driver' group by request_at
) as x2 on (x1.request_at = x2.request_at);

Create table If Not Exists Employee (Id int, Name varchar(255), Salary int, DepartmentId int);
Create table If Not Exists Department (Id int, Name varchar(255));
Truncate table Employee;
insert into Employee (Id, Name, Salary, DepartmentId) values ('1', 'Joe', '70000', '1');
insert into Employee (Id, Name, Salary, DepartmentId) values ('2', 'Henry', '80000', '2');
insert into Employee (Id, Name, Salary, DepartmentId) values ('3', 'Sam', '60000', '2');
insert into Employee (Id, Name, Salary, DepartmentId) values ('4', 'Max', '90000', '1');
Truncate table Department;
insert into Department (Id, Name) values ('1', 'IT');
insert into Department (Id, Name) values ('2', 'Sales');

select x1.dname as dept, x1.departmentid as deptid, x1.id, x1.name, x1.salary from (
  select e.id, d.name as dname, e.departmentid, e.name, e.salary from employee as e join department as d on (e.departmentid = d.id)
) as x1
where x1.salary >= (
  select x3.salary from employee as x3 where x3.departmentid = x1.departmentid order by x3.salary desc limit 1, 1
)
order by x1.departmentid asc, x1.salary desc;
