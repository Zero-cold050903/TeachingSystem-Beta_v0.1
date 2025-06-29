#ifndef COURSES_H
#define COURSES_H


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define FULLGRADE 100
#define MAXSTUDENT 100
#define COURSE_BASE "./data/course.txt"

typedef CourseNode;
typedef CourseGraph;
typedef Student;
typedef Course;
typedef ProgramStructure;

void viewEnrolledStudents(Course* course);
void viewPrerequisites(Course* course);
void shortestPathToConcept(CourseGraph* graph, Course* start, Course* target);
void addCourse(CourseGraph* graph, Course* course);
void deleteCourse(CourseGraph* graph, Course* course);
void updateCourse(Course* course, const char* newCourseName, const char* newDepartment,const char* newMajor, int newCreditHours);
Course* findCourseById(CourseGraph* graph, int id);
void findCoreCourses(CourseGraph* graph, ProgramStructure* program) ;


#endif;
