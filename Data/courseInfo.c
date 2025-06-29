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
void mainMenu() {
    CourseGraph* graph = (CourseGraph*)malloc(sizeof(CourseGraph));
    graph->capacity = 100;
    graph->size = 0;
    graph->courses = (Course**)calloc(graph->capacity, sizeof(Course*));

    ProgramStructure* program = NULL;

    int choice;
    do {
        printf("\n========== 教学管理系统 ==========\n");
        printf("1. 添加课程\n");
        printf("2. 删除课程\n");
        printf("3. 修改课程信息\n");
        printf("4. 查询课程信息\n");
        printf("5. 查看选课学生\n");
        printf("6. 查看先修课程\n");
        printf("7. 学习路径最短路径查询\n");
        printf("8. 保存课程结构到文件\n");
        printf("9. 从文件加载课程结构\n");
        printf("10. 查找专业核心课程\n");
        printf("0. 退出系统\n");
        printf("请输入操作编号: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                // 示例添加课程
                Course* course = (Course*)malloc(sizeof(Course));
                course->courseId = 101;
                course->courseName = strdup("数据结构");
                course->department = strdup("计算机学院");
                course->major = strdup("软件工程");
                course->creditHours = 4;
                course->prerequisites = NULL;
                course->enrolledStudents = NULL;
                course->studentCount = 0;

                addCourse(graph, course);
                printf("课程添加成功。\n");
                break;
            }
            case 2: {
                int id;
                printf("请输入要删除的课程ID: ");
                scanf("%d", &id);
                Course* target = findCourseById(graph, id);
                if (target != NULL) {
                    deleteCourse(graph, target);
                    printf("课程已删除。\n");
                } else {
                    printf("未找到该课程。\n");
                }
                break;
            }
            case 3: {
                int id;
                printf("请输入要修改的课程ID: ");
                scanf("%d", &id);
                Course* target = findCourseById(graph, id);
                if (target != NULL) {
                    updateCourse(target, "高级数据结构", "计算机学院", "软件工程", 4);
                    printf("课程信息已更新。\n");
                } else {
                    printf("未找到该课程。\n");
                }
                break;
            }
            case 4: {
                int id;
                printf("请输入要查询的课程ID: ");
                scanf("%d", &id);
                Course* target = findCourseById(graph, id);
                if (target != NULL) {
                    printf("课程名称: %s\n", target->courseName);
                    printf("所属学院: %s\n", target->department);
                    printf("所属专业: %s\n", target->major);
                    printf("学分: %d\n", target->creditHours);
                } else {
                    printf("未找到该课程。\n");
                }
                break;
            }
            case 5: {
                int id;
                printf("请输入课程ID以查看选课学生: ");
                scanf("%d", &id);
                Course* target = findCourseById(graph, id);
                if (target != NULL) {
                    viewEnrolledStudents(target);
                } else {
                    printf("未找到该课程。\n");
                }
                break;
            }
            case 6: {
                int id;
                printf("请输入课程ID以查看先修课程: ");
                scanf("%d", &id);
                Course* target = findCourseById(graph, id);
                if (target != NULL) {
                    viewPrerequisites(target);
                } else {
                    printf("未找到该课程。\n");
                }
                break;
            }
            case 7: {
                int startId, targetId;
                printf("请输入起始课程ID: ");
                scanf("%d", &startId);
                printf("请输入目标课程ID: ");
                scanf("%d", &targetId);

                Course* start = findCourseById(graph, startId);
                Course* target = findCourseById(graph, targetId);

                if (start && target) {
                    shortestPathToConcept(graph, start, target);
                } else {
                    printf("起始或目标课程不存在。\n");
                }
                break;
            }
            case 8: {
                if (program == NULL) {
                    program = (ProgramStructure*)malloc(sizeof(ProgramStructure));
                    program->department = strdup("计算机学院");
                    program->major = strdup("软件工程");
                    program->courses = graph->courses;
                    program->count = graph->size;
                }
                saveProgramStructureToFile(program, "software_engineering_courses.json");
                printf("课程结构已保存到文件。\n");
                break;
            }
            case 9: {
                if (program != NULL) {
                    free(program);
                }
                program = loadProgramStructureFromFile("software_engineering_courses.json");
                if (program != NULL) {
                    graph->courses = program->courses;
                    graph->size = program->count;
                    printf("课程结构已从文件加载。\n");
                } else {
                    printf("加载失败，请检查文件是否存在。\n");
                }
                break;
            }
            case 10: {
                if (program == NULL) {
                    printf("请先加载或创建专业课程结构。\n");
                    break;
                }
                findCoreCourses(graph, program);
                break;
            }
            case 0:
                printf("退出系统。\n");
                break;
            default:
                printf("无效的操作编号，请重新输入。\n");
        }
    } while (choice != 0);

    // 清理资源
    for (int i = 0; i < graph->size; i++) {
        free(graph->courses[i]);
    }
    free(graph->courses);
    free(graph);
    if (program != NULL) {
        free(program);
    }

    exit(0);
}
