#include "BranchGenerator.h"
#include "../Settings.h"

ProceduralTree::BranchGenerator::BranchGenerator(TreeBranchSettings settings)
    : m_settings{settings}, m_grid{false} {}

ProceduralTree::BranchGenerator::~BranchGenerator() {
  for (size_t i = 0; i < m_nodes.size(); i++) {
    delete m_nodes[i];
  }
}

std::vector<ProceduralTree::Branch>
ProceduralTree::BranchGenerator::getLastGeneratedBranches() {
  return m_branches;
}

void ProceduralTree::BranchGenerator::generateAttractionPoints(
    glm::ivec3 tree_pos) {
  std::mt19937 generator_x(tree_pos.x);
  std::mt19937 generator_y(tree_pos.y);
  std::mt19937 generator_z(tree_pos.z);

  double range = 3.0;

  std::uniform_real_distribution<double> dist(-range, range);
  std::uniform_real_distribution<double> dist_unsigned(0, range);
  for (size_t x = 0; x < Settings::CHUNK_SIZE; x += 4) {
    for (size_t y = 0; y < Settings::CHUNK_SIZE; y += 4) {
      for (size_t z = 0; z < Settings::CHUNK_SIZE; z += 4) {
        glm::vec3 ap_pos =
            tree_pos + glm::ivec3(dist(generator_x),
                                  dist_unsigned(generator_y) + range,
                                  dist(generator_z));
        AttractionPoint ap{ap_pos, false, m_settings.attraction_point_range};
        m_attraction_points.push_back(ap);
      }
    }
  }
}

std::vector<ProceduralTree::Branch>
ProceduralTree::BranchGenerator::generateBranches(glm::ivec3 tree_pos) {
  // TODO: Make this stateless instead of clearing each time
  m_branches.clear();
  m_nodes.clear();
  m_attraction_points.clear();
  std::queue<Node*> empty;
  std::swap(m_pending_new_nodes, empty);
  // ---- 
  generateAttractionPoints(tree_pos);
  createRootNode(tree_pos);
  size_t iter_count = 0;
  for (size_t i = 0;
       i < m_settings.max_iterations && atLeastOneAttractionPointExists() &&
       nodesAreInAttractionPointsRange();
       i++) {
    doIteration();
  }
  printBranches();
  return m_branches;
}

void ProceduralTree::BranchGenerator::doIteration() {
  for (auto& ap : m_attraction_points) {
    int closest_node_index = -1;
    size_t closest_distance = 99999;
    for (int i = 0; i < m_nodes.size(); i++) {
      Node* v = m_nodes[i];
      size_t distance = calculateDistance(ap, *v);

      if (distance < closest_distance) {
        closest_distance = distance;
        closest_node_index = i;
      }
    }

    ap.reached = closest_distance < m_settings.kill_distance;
    if (closest_distance < ap.radius && closest_node_index != -1) {
      m_nodes[closest_node_index]->attraction_points_in_range.insert(ap);
      m_pending_new_nodes.push(m_nodes[closest_node_index]);
    }
  }

  createNewNodes();
  removeAttractionPointsInKillDistance();

  for (auto& node : m_nodes) {
    node->attraction_points_in_range.clear();
  }
}

void ProceduralTree::BranchGenerator::createRootNode(glm::ivec3 tree_pos) {
  Node* root = new Node(
      tree_pos,
      std::unordered_set<AttractionPoint, AttractionPoint::HashFunction>(),
      nullptr, std::vector<Node*>());
  m_nodes.push_back(root);
}

void ProceduralTree::BranchGenerator::createNewNodes() {
  while (!m_pending_new_nodes.empty()) {
    createNewNode(m_pending_new_nodes.front());
    m_pending_new_nodes.pop();
  }
}

void ProceduralTree::BranchGenerator::createNewNode(Node* parent) {
  glm::vec3 direction{0, 0, 0};
  for (auto& ap : parent->attraction_points_in_range) {
    direction += glm::normalize(calculateDistanceVec(ap, *parent));
  }
  direction = glm::normalize(direction);

  glm::vec3 scaled_direction = {direction.x * m_settings.branch_length,
                                direction.y * m_settings.branch_length,
                                direction.z * m_settings.branch_length};
  glm::vec3 new_node_pos = parent->pos + scaled_direction;

  glm::ivec3 new_node_ipos = glm::ivec3(new_node_pos);
  if (m_grid.contains(new_node_ipos)) {
    return;
  }
  m_grid.insert(new_node_ipos);
  Node* child = new Node(
      new_node_pos,
      std::unordered_set<AttractionPoint, AttractionPoint::HashFunction>(),
      parent, std::vector<Node*>());

  parent->childs.push_back(child);
  m_branches.push_back({parent, child});
  m_nodes.push_back(child);
}

void ProceduralTree::BranchGenerator::removeAttractionPointsInKillDistance() {
  m_attraction_points.erase(
      std::remove_if(
          begin(m_attraction_points), end(m_attraction_points),
          [](AttractionPoint const& ap) { return ap.reached == true; }),
      m_attraction_points.end());
}

uint32_t ProceduralTree::BranchGenerator::calculateDistance(AttractionPoint ap,
                                                            Node node) {
  return glm::length(calculateDistanceVec(ap, node));
}

glm::vec3 ProceduralTree::BranchGenerator::calculateDistanceVec(
    AttractionPoint ap, Node node) {
  return ap.pos - node.pos;
}

bool ProceduralTree::BranchGenerator::atLeastOneAttractionPointExists() {
  return m_attraction_points.size() > 0;
}

bool ProceduralTree::BranchGenerator::nodesAreInAttractionPointsRange() {
  return true;
}

void ProceduralTree::BranchGenerator::printBranches() {
  LOG_F(INFO, "-----------PRINT BRANCHES------------");
  for (auto& branch : m_branches) {
    glm::vec3 v = branch.v->pos;
    glm::vec3 u = branch.u->pos;
    LOG_F(INFO, "branch  pos: (%f, %f, %f) -> (%f, %f, %f)", v.x, v.y, v.z, u.x,
          u.y, u.z);
    auto v_childs = branch.v->childs;
    for (auto v_child : v_childs) {
      LOG_F(INFO, "v_child pos (%f, %f, %f)", v_child->pos.x,
            v_child->pos.y, v_child->pos.z);
    }
    auto u_childs = branch.u->childs;
    for (auto u_child : u_childs) {
      LOG_F(INFO, "u_child pos (%f, %f, %f)", u_child->pos.x, u_child->pos.y,
            u_child->pos.z);
    }
    LOG_F(INFO, "branch v_childs=%d, u_childs=%d", v_childs.size(), u_childs.size());
  }
  LOG_F(INFO, "----------------------------");
}
