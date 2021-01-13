/*
 * Exemple d'utilisation de la librairie libxml++ version 1.0.
 */

#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include <WARMUX_base.h>
#include <string>
#include <vector>

// Forward declaration
typedef struct _xmlNode xmlNode;
typedef struct _xmlDoc xmlDoc;
typedef std::vector<const xmlNode*> xmlNodeArray;
class Color;

class XmlReader
{
  xmlDoc*  doc;
public:
  XmlReader() : doc(NULL) { };
  ~XmlReader();

  // Load an XML document from a file
  bool Load(const std::string &nomfich);

  // Load an XML document from a string in memory
  bool LoadFromString(const std::string &contents);
  std::string ExportToString() const;

  bool IsOk() const { return doc != NULL; }

  // Return the *exploitable* root (use root->parent for the real one) */
  const xmlNode* GetRoot() const;

  // Return the direct children matching name
  static xmlNodeArray GetNamedChildren(const xmlNode* father,
                                       const std::string & name);

  // Return the first child node element named "nodeName" of "father".
  const xmlNode * GetFirstNamedChild(const xmlNode * father,
                                     const std::string & nodeName);

  // Return the current number of children nodes of "father" node.
  uint GetNbChildren(const xmlNode * father);

  // Return the first child node of "father", otherwise NULL
  const xmlNode * GetFirstChild(const xmlNode * father);

  // Return the first next sibling element of "node", otherwise NULL
  const xmlNode * GetNextSibling(const xmlNode * node);

  // Return the name of "node"
  std::string GetNodeName(const xmlNode * node);

  // get a attribute marker
  static bool ReadString(const xmlNode* father,
                         const std::string &name,
                         std::string &output);
  static bool ReadDouble(const xmlNode* father,
                         const std::string &name,
                         Double &output);
  static bool Readfloat(const xmlNode* father,
                        const std::string &name,
                        float &output);
  static bool ReadInt(const xmlNode* father,
                      const std::string &name,
                      int &output);
  static bool ReadUint(const xmlNode* father,
                       const std::string &name,
                       uint &output);
  static bool ReadBool(const xmlNode* father,
                       const std::string &name,
                       bool &output);

  // get an XML element
  static const xmlNode* GetMarker(const xmlNode* x,
                                  const std::string &name);

  // Access to the 'anchor' <[name] name="[attr_name]"> : have to be uniq !
  static const xmlNode* Access(const xmlNode* x,
                               const std::string &name,
                               const std::string &attr_name);

  // Lit un attribut d'un noeud
  static bool ReadStringAttr(const xmlNode* x,
                             const std::string &name,
                             std::string &output);
  static bool ReadDoubleAttr(const xmlNode* x,
                             const std::string &name,
                             Double &output);
  static bool ReadfloatAttr(const xmlNode* x,
                            const std::string &name,
                            float &output);
  static bool ReadIntAttr(const xmlNode* x,
                          const std::string &name,
                          int &output);
  bool ReadPercentageAttr(const xmlNode* node,
                          const std::string & attributName,
                          float & outputValue);
  bool ReadPixelAttr(const xmlNode* node,
                     const std::string & attributName,
                     int & outputValue);
  static bool ReadUintAttr(const xmlNode* x,
                           const std::string &name,
                           uint &output);
  static bool ReadBoolAttr(const xmlNode* x,
                           const std::string &name,
                           bool &output);
  bool ReadHexColorAttr(const xmlNode* node,
                        const std::string & attributName,
                        Color & outputColor);
  bool IsAPercentageAttr(const xmlNode* node,
                         const std::string & attributName);

private:
  // Read marker value
  static bool ReadMarkerValue(const xmlNode* marker, std::string &output);
  void Reset();
};

//-----------------------------------------------------------------------------

class XmlWriter
{
  void Reset();

protected:
  xmlDoc*  m_doc;
  xmlNode* m_root;
  std::string m_filename;
  bool m_save;
  std::string m_encoding;

public:
  XmlWriter() : m_doc(NULL), m_root(NULL), m_save(false) { } ;
  ~XmlWriter();

  bool Create(const std::string &filename, const std::string &root,
              const std::string &version, const std::string &encoding);

  bool IsOk() const { return m_doc && m_root; }

  xmlNode *GetRoot() const;

  xmlNode *WriteElement(xmlNode* x,
                        const std::string &name,
                        const std::string &value);

  void WriteComment(xmlNode* x,
                    const std::string& comment);

  bool Save();

  std::string SaveToString() const;
};

#endif /* XML_DOCUMENT_H */
