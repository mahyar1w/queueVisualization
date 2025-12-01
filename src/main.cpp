#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

static bool g_first_frame = true;
static const int MAX_SIZE = 5;
static int queue[MAX_SIZE] = { 0 };
static int count = 0;
static int dequeued_val = 0;
static float message_timer = 0.0f;
static const float MESSAGE_DURATION = 1.5f;
static bool g_discard_message = false;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = nullptr;
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #else
        glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(400 * main_scale), (int)(300 * main_scale), "Dear ImGui Shifting History Buffer", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    static int buf = 10; 

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        if (message_timer > 0.0f)
        {
            message_timer -= ImGui::GetIO().DeltaTime;
        }
        else
        {
            g_discard_message = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGuiWindowFlags window_flags =
                  ImGuiWindowFlags_NoDocking
                | ImGuiWindowFlags_NoTitleBar
                | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoMove
                | ImGuiWindowFlags_NoBringToFrontOnFocus
                | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoBackground;

            ImGui::Begin("Main Application Space", nullptr, window_flags);

            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

            if (g_first_frame)
            {
                ImGui::DockBuilderRemoveNode(dockspace_id); 
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
                ImGui::DockBuilderDockWindow("Main Content", dockspace_id);
                ImGui::DockBuilderFinish(dockspace_id);
                g_first_frame = false;
            }
            ImGui::Begin("Main Content"); 
            
            ImGui::Separator();
            ImGui::InputInt("Value to Enqueue", &buf, 0, 0, ImGuiInputTextFlags_None);

            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            if (ImGui::Button("Enqueue"))
            {
                g_discard_message = (count == MAX_SIZE);
                
                if (count == MAX_SIZE)
                {
                    dequeued_val = queue[MAX_SIZE - 1];
                    
                    for (int i = MAX_SIZE - 1; i > 0; --i)
                    {
                        queue[i] = queue[i - 1];
                    }
                } 
                else 
                {
                    for (int i = count; i > 0; --i)
                    {
                        queue[i] = queue[i - 1];
                    }
                    count++;
                    dequeued_val = 0;
                }

                queue[0] = buf;
                message_timer = MESSAGE_DURATION;
            }

            ImGui::SameLine(0.0f, 60.0f);
            
            if (ImGui::Button("Dequeue (Remove Last)"))
            {
                if (count > 0)
                {
                    dequeued_val = queue[count - 1];
                    queue[count - 1] = 0;
                    count--;
                    message_timer = MESSAGE_DURATION;
                    g_discard_message = false;
                }
                else
                {
                    message_timer = MESSAGE_DURATION;
                    g_discard_message = false;
                }
            }
    
            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            if (message_timer > 0.0f)
            {
                if (g_discard_message)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "Buffer was FULL. Discarded Oldest (%d) to insert Newest (%d).", dequeued_val, buf);
                }
                else if (dequeued_val != 0 && !g_discard_message)
                {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Successfully Dequeued: %d", dequeued_val);
                    dequeued_val = 0;
                }
                else if (count == 0 && ImGui::IsItemHovered() == false)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Queue is EMPTY! Cannot dequeue.");
                }
                else if (ImGui::IsItemHovered() == false)
                {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Successfully Enqueued: %d", buf);
                }
            }


            ImGui::Text("Current Count: %d / %d", count, MAX_SIZE);
            ImGui::Dummy(ImVec2(0.0f, 4.0f));


            const int COLUMNS = MAX_SIZE;
            if (ImGui::BeginTable("Queue_Table", COLUMNS, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame))
            {
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                for (int i = 0; i < COLUMNS; i++)
                {
                    ImGui::TableSetColumnIndex(i);
                    ImGui::Text("Index %d", i);
                }

                ImGui::TableNextRow();
                
                for (int i = 0; i < COLUMNS; i++)
                {
                    ImGui::TableSetColumnIndex(i);
                    
                    if (i == 0 && count > 0)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.2f, 1.0f))); 
                    else if (i == count - 1 && count > 0)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.6f, 0.2f, 0.2f, 1.0f)));
                    
                    if (queue[i] != 0 || i < count)
                    {
                        ImGui::Text("%d", queue[i]);
                    }
                    else
                    {
                        ImGui::Text("...");
                    }
                }
                
                ImGui::EndTable();
            }
            ImGui::End();
            ImGui::End();

        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}