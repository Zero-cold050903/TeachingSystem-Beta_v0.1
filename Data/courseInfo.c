#include "courseInfo.h"
struct Student {
    int studentId;
    char* name;
};
struct Course {
    int courseId;
    char* courseName;
    char* department; // 学院
    char* major;      // 专业
    int creditHours;
    struct CourseNode* prerequisites; // 先修课程（有向边）
    Student** enrolledStudents;       // 选课学生
    int studentCount;
};
struct CourseNode {
    Course* course;
    struct CourseNode* next;
};

struct CourseGraph{
    Course** courses;
    int capacity;
    int size;
};
struct Student {
    int studentId;
    char* name;
};
struct ProgramStructure {
    char* department;
    char* major;
    Course** courses;
    int count;
};
void viewEnrolledStudents(Course* course) {
    printf("Students enrolled in %s:\n", course->courseName);
    for (int i = 0; i < course->studentCount; i++) {
        printf("%d: %s\n", course->enrolledStudents[i]->studentId, course->enrolledStudents[i]->name);
    }
}
void viewPrerequisites(Course* course) {
    printf("Prerequisites for %s:\n", course->courseName);
    CourseNode* node = course->prerequisites;
    while (node != NULL) {
        printf("- %s\n", node->course->courseName);
        node = node->next;
    }
}
// Build distance map from source course to all others
void shortestPathToConcept(CourseGraph* graph, Course* start, Course* target) {
    int V = graph->size;
    int* dist = malloc(V * sizeof(int));
    int* visited = calloc(V, sizeof(int));

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
    }

    int startIndex = -1;
    for (int i = 0; i < V; i++) {
        if (graph->courses[i] == start) {
            startIndex = i;
            break;
        }
    }

    if (startIndex == -1) {
        printf("Start course not found.\n");
        return;
    }

    dist[startIndex] = 0;
    for (int count = 0; count < V - 1; count++) {
        int u = -1;
        for (int v = 0; v < V; v++) {
            if (!visited[v] && (u == -1 || dist[v] < dist[u])) {
                u = v;
            }
        }

        if (dist[u] == INT_MAX) break;

        visited[u] = 1;
        CourseNode* edge = graph->courses[u]->prerequisites;
        while (edge != NULL) {
            int v = -1;
            for (int i = 0; i < V; i++) {
                if (graph->courses[i] == edge->course) {
                    v = i;
                    break;
                }
            }
            if (v != -1 && !visited[v] && dist[u] + 1 < dist[v]) {
                dist[v] = dist[u] + 1;
            }
            edge = edge->next;
        }
    }

    int targetIndex = -1;
    for (int i = 0; i < V; i++) {
        if (graph->courses[i] == target) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex != -1 && dist[targetIndex] != INT_MAX) {
        printf("Shortest path from %s to %s: %d steps\n",
               start->courseName, target->courseName, dist[targetIndex]);
    } else {
        printf("No path exists from %s to %s\n", start->courseName, target->courseName);
    }

    free(dist);
    free(visited);
}
void addCourse(CourseGraph* graph, Course* course) {
    if (graph->size < graph->capacity) {
        graph->courses[graph->size++] = course;
    } else {
        printf("Course capacity reached.\n");
    }
}void deleteCourse(CourseGraph* graph, Course* course) {
    for (int i = 0; i < graph->size; i++) {
        if (graph->courses[i] == course) {
            for (int j = i; j < graph->size - 1; j++) {
                graph->courses[j] = graph->courses[j + 1];
            }
            graph->size--;
            break;
        }
    }
}
void updateCourse(Course* course, const char* newCourseName, const char* newDepartment,
                  const char* newMajor, int newCreditHours) {
    course->courseName = strdup(newCourseName);
    course->department = strdup(newDepartment);
    course->major = strdup(newMajor);
    course->creditHours = newCreditHours;
}
Course* findCourseById(CourseGraph* graph, int id) {
        for (int i = 0; i < graph->size; i++) {
        if (graph->courses[i]->courseId == id) {
            return graph->courses[i];
        }
    }
    return NULL;
}
void saveProgramStructureToFile(ProgramStructure* ps, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("无法打开文件进行写入");
        return;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"department\": \"%s\",\n", ps->department);
    fprintf(file, "  \"major\": \"%s\",\n", ps->major);
    fprintf(file, "  \"courses\": [\n");

    for (int i = 0; i < ps->count; i++) {
        Course* course = ps->courses[i];
        fprintf(file, "    {\n");
        fprintf(file, "      \"courseId\": %d,\n", course->courseId);
        fprintf(file, "      \"courseName\": \"%s\",\n", course->courseName);
        fprintf(file, "      \"creditHours\": %d,\n", course->creditHours);

        // Prerequisites
        fprintf(file, "      \"prerequisites\": [");
        CourseNode* preReq = course->prerequisites;
        int first = 1;
        while (preReq) {
            if (!first) fprintf(file, ", ");
            fprintf(file, "%d", preReq->course->courseId);
            first = 0;
            preReq = preReq->next;
        }
        fprintf(file, "],\n");

        // Enrolled students
        fprintf(file, "      \"students\": [\n");
        for (int j = 0; j < course->studentCount; j++) {
            Student* student = course->enrolledStudents[j];
            fprintf(file, "        {\"studentId\": %d, \"name\": \"%s\"}", student->studentId, student->name);
            if (j != course->studentCount - 1) fprintf(file, ",");
            fprintf(file, "\n");
        }
        fprintf(file, "      ]\n");

        if (i != ps->count - 1) fprintf(file, "    },\n");
        else fprintf(file, "    }\n");
    }

    fprintf(file, "  ]\n");
    fprintf(file, "}\n");

    fclose(file);
}
ProgramStructure* loadProgramStructureFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("无法打开文件进行读取");
        return NULL;
    }

    // 简化处理：只用于演示，实际应使用更鲁棒的解析方式
    ProgramStructure* ps = (ProgramStructure*)malloc(sizeof(ProgramStructure));
    char line[1024];

    // 读取部门和专业
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "\"department\"")) {
            sscanf(line, "  \"department\": \"%[^\"]\"", ps->department);
        } else if (strstr(line, "\"major\"")) {
            sscanf(line, "  \"major\": \"%[^\"]\"", ps->major);
            break;
        }
    }

    ps->courses = (Course**)malloc(100 * sizeof(Course*));
    ps->count = 0;

    // TODO: 实现完整课程结构的解析

    fclose(file);
    return ps;
}
/*
Example info structure
{
  "department": "计算机学院",
  "major": "软件工程",
  "courses": [
    {
      "courseId": 101,
      "courseName": "数据结构",
      "creditHours": 4,
      "prerequisites": [],
      "students": []
    },
    {
      "courseId": 201,
      "courseName": "算法设计",
      "creditHours": 3,
      "prerequisites": [101],
      "students": [
        {"studentId": 1001, "name": "张三"},
        {"studentId": 1002, "name": "李四"}
      ]
    }
  ]
}
*/
void findCoreCourses(CourseGraph* graph, ProgramStructure* program) {
    printf("Core courses in %s - %s:\n", program->department, program->major);

    int* dependencyCount = calloc(graph->size, sizeof(int));

    for (int i = 0; i < graph->size; i++) {
        Course* course = graph->courses[i];
        CourseNode* dep = course->prerequisites;
        while (dep != NULL) {
            for (int j = 0; j < graph->size; j++) {
                if (graph->courses[j] == dep->course) {
                    dependencyCount[j]++;
                    break;
                }
            }
            dep = dep->next;
        }
    }

    // Sort by dependency count
    for (int i = 0; i < graph->size; i++) {
        for (int j = i + 1; j < graph->size; j++) {
            if (dependencyCount[i] < dependencyCount[j]) {
                int temp = dependencyCount[i];
                dependencyCount[i] = dependencyCount[j];
                dependencyCount[j] = temp;

                Course* tmpC = graph->courses[i];
                graph->courses[i] = graph->courses[j];
                graph->courses[j] = tmpC;
            }
        }
    }

    // Print top 3 core courses
    for (int i = 0; i < 3 && i < graph->size; i++) {
        printf("- %s (%d dependencies)\n", graph->courses[i]->courseName, dependencyCount[i]);
    }

    free(dependencyCount);
}