#pragma once
#include "../chunk/Chunk.h"
#include <queue>
#include <unordered_set>
#include <random>

// http://algorithmicbotany.org/papers/colonization.egwnp2007.large.pdf

namespace ProceduralTree
{
	struct AttractionPoint
	{
		glm::vec3 pos;
		bool reached;
		uint8_t radius;

		struct HashFunction
		{
			size_t operator()(const AttractionPoint& ap) const
			{
				size_t xHash = std::hash<size_t>()(ap.pos.x);
				size_t yHash = std::hash<size_t>()(ap.pos.y) << 1;
				size_t zHash = std::hash<size_t>()(ap.pos.y) << 2;
				return xHash ^ yHash ^ zHash;
			}
		};

		bool operator==(const AttractionPoint& other) const
		{
			if (this->pos == other.pos && this->reached == other.reached && this->radius == other.radius) return true;
			else return false;
		}
	};

	struct Node
	{
		glm::vec3 pos;
		std::unordered_set<AttractionPoint, AttractionPoint::HashFunction> attraction_points_in_range;
		Node* parent;
		std::vector<Node*> childs;

		Node(glm::vec3 o_pos, std::unordered_set<AttractionPoint, AttractionPoint::HashFunction> o_ap, Node* o_parent, std::vector<Node*> o_childs)
			: pos{o_pos},
			attraction_points_in_range{o_ap},
			parent{o_parent},
			childs{o_childs}
		{

		}

		Node()
		{
			pos = glm::vec3(0, 0, 0);
			attraction_points_in_range = std::unordered_set<AttractionPoint, AttractionPoint::HashFunction>();
			parent = nullptr;
			childs = std::vector<Node*>();
		};

		virtual ~Node() = default;

		Node(const Node& o)
		{
			pos = o.pos;
			attraction_points_in_range = o.attraction_points_in_range;
			parent = o.parent;
			childs = o.childs;
		}
	};

	struct Branch
	{
		Node* v;
		Node* u;

		Branch(Node* o_v, Node* o_u)
			: 
			v{o_v},
			u{o_u}
		{}

		Branch(const Branch& o)
		{
			v = o.v;
			u = o.u;
		}
	};

	struct Settings
	{
		unsigned int kill_distance; // choosing multiples of branch_length is recommended in the paper
		unsigned int branch_length;
		unsigned int attraction_point_range;
		unsigned int max_attraction_points;
		unsigned int max_iterations;
		unsigned int bounding_box_size;
	};

	static const Settings DefaultSettings
	{
		.kill_distance{4},
		.branch_length{2},
		.attraction_point_range{8},
		.max_attraction_points{3200},
		.max_iterations{320},
		.bounding_box_size{16}
	};

	class BranchGenerator
	{
	public:
		BranchGenerator(Settings settings = DefaultSettings);
		virtual ~BranchGenerator();
		std::vector<Branch> generateBranches(glm::ivec3 tree_pos);
		std::vector<Branch> getLastGeneratedBranches();
		void printBranches();

	private:
		void generateAttractionPoints(glm::ivec3 tree_pos);

		void doIteration();
		void createRootNode(glm::ivec3 tree_pos);
		void createNewNode(Node* parent);
		void createNewNodes();
		void removeAttractionPointsInKillDistance();

		uint32_t calculateDistance(AttractionPoint ap, Node node);
		glm::vec3 calculateDistanceVec(AttractionPoint ap, Node node);
		bool atLeastOneAttractionPointExists();
		bool maxIterationsNotReached();
		bool nodesAreInAttractionPointsRange();


		std::vector<AttractionPoint> m_attraction_points;
		std::vector<Node*> m_nodes;
		std::vector<Branch> m_branches;

		std::queue<Node*> m_pending_new_nodes;
		Settings m_settings;
		std::unordered_set<glm::ivec3> m_grid;
	};
}

