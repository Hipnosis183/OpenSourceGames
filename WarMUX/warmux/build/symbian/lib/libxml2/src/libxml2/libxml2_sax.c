/*
 * libxml2_sax.c : Old SAX v1 handlers to build a tree.
 *                 Deprecated except for compatibility
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <daniel@veillard.com>
 */

#define IN_LIBXML

#include "xmlenglibxml.h"

#include <stdlib.h>
#include <string.h>

#include <libxml2_parserinternals.h>
#include <libxml2_uri.h>
#include <libxml2_globals.h>

#ifdef LIBXML_HTML_ENABLED
#include "libxml2_htmltree.h" 
#endif

#ifdef LIBXML_DEBUG_ENABLED
#include "libxml2_debugxml.h"
#endif

#ifdef LIBXML_SAX1_ENABLED
/**
 * initxmlDefaultSAXHandler:
 * @param hdlr the SAX handler
 * @param warning flag if non-zero sets the handler warning procedure
 *
 * Initialize the default XML SAX version 1 handler
 * DEPRECATED: use xmlSAX2InitDefaultSAXHandler() for the new SAX2 blocks
 */
XMLPUBFUNEXPORT void
initxmlDefaultSAXHandler(xmlSAXHandlerV1 *hdlr, int warning)
{
    if(hdlr->initialized == 1)
        return;

    hdlr->internalSubset = xmlSAX2InternalSubset;
    hdlr->externalSubset = xmlSAX2ExternalSubset;
    hdlr->isStandalone = xmlSAX2IsStandalone;
    hdlr->hasInternalSubset = xmlSAX2HasInternalSubset;
    hdlr->hasExternalSubset = xmlSAX2HasExternalSubset;
    hdlr->resolveEntity = xmlSAX2ResolveEntity;
    hdlr->getEntity = xmlSAX2GetEntity;
    hdlr->getParameterEntity = xmlSAX2GetParameterEntity;
    hdlr->entityDecl = xmlSAX2EntityDecl;
    hdlr->attributeDecl = xmlSAX2AttributeDecl;
    hdlr->elementDecl = xmlSAX2ElementDecl;
    hdlr->notationDecl = xmlSAX2NotationDecl;
    hdlr->unparsedEntityDecl = xmlSAX2UnparsedEntityDecl;
    hdlr->setDocumentLocator = xmlSAX2SetDocumentLocator;
    hdlr->startDocument = xmlSAX2StartDocument;
    hdlr->endDocument = xmlSAX2EndDocument;
    hdlr->startElement = xmlSAX2StartElement;
    hdlr->endElement = xmlSAX2EndElement;
    hdlr->reference = xmlSAX2Reference;
    hdlr->characters = xmlSAX2Characters;
    hdlr->cdataBlock = xmlSAX2CDataBlock;
    hdlr->ignorableWhitespace = xmlSAX2Characters;
    hdlr->processingInstruction = xmlSAX2ProcessingInstruction;
    if (warning == 0)
        hdlr->warning = NULL;
    else
        hdlr->warning = xmlParserWarning;
    hdlr->error = xmlParserError;
    hdlr->fatalError = xmlParserError;

    hdlr->initialized = 1;
}
#endif /* LIBXML_SAX1_ENABLED */
