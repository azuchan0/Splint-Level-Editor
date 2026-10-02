#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Includes de notre moteur
#include "gwafix/Shader.hpp"
#include "gwafix/VertexBuffer.hpp"
#include "math/bezier.hpp"

#include <iostream>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <chrono> // Pour le calcul des performances

// Structure pour garder l'état de l'éditeur
struct EditorState {
    Bezier* currentCurve;
    int selectedPoint = -1;    // Index du point qu'on est en train de cliquer/glisser
    int resolution = 50;       // Le "pas" de la courbe
    bool needsUpdate = true;   // Faut-il recalculer la géométrie ?
};

// --- CALLBACKS GLFW ---

// Clic de la souris
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; // Ne rien faire si on clique sur une fenêtre ImGui !

    EditorState* state = (EditorState*)glfwGetWindowUserPointer(window);
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glm::vec2 mousePos(xpos, ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // 1. Vérifier si on clique sur un point existant (rayon de 15 pixels)
            state->selectedPoint = -1;
            const auto& pts = state->currentCurve->getControlPoints();
            for (int i = 0; i < pts.size(); ++i) {
                if (glm::length(pts[i] - mousePos) < 15.0f) {
                    state->selectedPoint = i;
                    break;
                }
            }

            // 2. Si on n'a cliqué sur aucun point, on en ajoute un nouveau
            if (state->selectedPoint == -1) {
                state->currentCurve->addControlPoint(mousePos);
                state->selectedPoint = (int)state->currentCurve->getControlPoints().size() - 1;
                state->needsUpdate = true;
            }
        }
        else if (action == GLFW_RELEASE) {
            state->selectedPoint = -1; // On lâche le point
        }
    }
}

// Glissement de la souris
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    EditorState* state = (EditorState*)glfwGetWindowUserPointer(window);
    if (state->selectedPoint != -1) {
        state->currentCurve->setControlPoint(state->selectedPoint, glm::vec2(xpos, ypos));
        state->needsUpdate = true;
    }
}

// Touches du clavier (+ et -)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    EditorState* state = (EditorState*)glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL) { // Touche +
            state->resolution += 5;
            state->needsUpdate = true;
        }
        if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) { // Touche -
            if (state->resolution > 5) state->resolution -= 5;
            state->needsUpdate = true;
        }
    }
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int windowWidth = 1280;
    int windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Spline Editor - Etape 1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader myShader("shaders/curve.vert", "shaders/curve.frag");
    VertexBuffer curveMesh;
    VertexBuffer controlPointsMesh;

    Bezier myBezier;

    // Initialisation de notre Etat
    EditorState state;
    state.currentCurve = &myBezier;
    glfwSetWindowUserPointer(window, &state);

    // Enregistrement des callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);

    double computationTime = 0.0;
    int currentMethod = 1; // 0 = Directe, 1 = Casteljau

    // --- BOUCLE PRINCIPALE ---
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // 1. Mise à jour Mathématique (Seulement si un point a bougé ou a été ajouté)
        if (state.needsUpdate) {
            // Choix de l'algorithme
            myBezier.setMethod(currentMethod == 0 ? Bezier::Method::DIRECT : Bezier::Method::CASTELJAU);

            // Profiling des performances
            auto start = std::chrono::high_resolution_clock::now();
            myBezier.generateVertices(state.resolution);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            computationTime = duration.count();

            // Envoi à la carte graphique
            curveMesh.setData(myBezier.getVertices());
            controlPointsMesh.setData(myBezier.getControlPoints());

            state.needsUpdate = false;
        }

        // 2. Début de la frame ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- INTERFACE DE L'ÉDITEUR ---
        ImGui::Begin("Toolbox : Spline Editor");
        ImGui::Text("Nombre de points : %d", (int)myBezier.getControlPoints().size());
        ImGui::SliderInt("Résolution (Pas)", &state.resolution, 5, 200);
        if (ImGui::IsItemEdited()) state.needsUpdate = true;

        ImGui::Separator();
        ImGui::Text("Algorithme de calcul :");
        if (ImGui::RadioButton("Formule Directe (Bernstein)", &currentMethod, 0)) state.needsUpdate = true;
        if (ImGui::RadioButton("Casteljau (Itératif)", &currentMethod, 1)) state.needsUpdate = true;

        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Temps de calcul : %.4f ms", computationTime);

        if (ImGui::Button("Vider la scène")) {
            myBezier.clear();
            state.needsUpdate = true;
        }
        ImGui::End();

        // 3. Rendu OpenGL
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
        myShader.bind();
        myShader.setMat4("u_Projection", projection);

        // Dessin des points et des lignes de contrôle
        myShader.setVec3("u_Color", glm::vec3(0.4f, 0.4f, 0.4f));
        controlPointsMesh.draw(GL_LINE_STRIP);

        glPointSize(10.0f);
        myShader.setVec3("u_Color", glm::vec3(0.9f, 0.2f, 0.3f));
        controlPointsMesh.draw(GL_POINTS);

        // Dessin de la courbe finale
        myShader.setVec3("u_Color", glm::vec3(0.2f, 0.8f, 1.0f)); // Bleu néon
        curveMesh.draw(GL_LINE_STRIP);

        // Rendu ImGui par dessus
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}